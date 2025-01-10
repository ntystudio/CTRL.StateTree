// Fill out your copyright notice in the Description page of Project Settings.

#include "EstChangeInputConfigSubsytem.h"

#include "TimerManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY(LogEstChangeInputConfigSubsystem)

#define CICS_LOG(Verbosity, ...) UE_LOG(LogEstChangeInputConfigSubsystem, Verbosity, ##__VA_ARGS__)
#define CICS_CLOG(Cond, Verbosity, ...) UE_CLOG(Cond, LogEstChangeInputConfigSubsystem, Verbosity, ##__VA_ARGS__)

FString LexToStringOptionalHandle(TOptional<FGuid> const& Guid)
{
	return Guid.IsSet() ? Guid.GetValue().ToString() : TEXT("None");
}

UEstChangeInputConfigSubsystem* UEstChangeInputConfigSubsystem::Get(ULocalPlayer const* LocalPlayer)
{
	return LocalPlayer ? LocalPlayer->GetSubsystem<UEstChangeInputConfigSubsystem>() : nullptr;
}

UEstChangeInputConfigSubsystem* UEstChangeInputConfigSubsystem::Get(APlayerController const* PC)
{
	return Get(PC ? PC->GetLocalPlayer() : nullptr);
}

TOptional<FEstInputModeConfig> UEstChangeInputConfigSubsystem::GetInputConfig(TOptional<FGuid> const& InputConfigHandle) const
{
	if (!InputConfigHandle.IsSet()) { return {}; }
	FGuid const Key = InputConfigHandle.GetValue();
	if (!InputConfigs.Contains(Key))
	{
		ensure(!InputConfigHandleStack.Contains(Key));
		ensure(!EnqueuedInputConfigs.Contains(Key));
		return {};
	}
	return InputConfigs[Key];
}

FGuid UEstChangeInputConfigSubsystem::PushInputConfig(FEstInputModeConfig const& InputConfig)
{
	FGuid const InputConfigHandle = FGuid::NewGuid();
	InputConfigHandleStack.Push(InputConfigHandle);
	InputConfigs.Add(InputConfigHandle, InputConfig);
	EnqueuedInputConfigs.Add(InputConfigHandle);
	ScheduleUpdate();
	return InputConfigHandle;
}

void UEstChangeInputConfigSubsystem::PopInputConfig(FGuid const& InputConfigHandle)
{
	if (!ensure(InputConfigHandleStack.Contains(InputConfigHandle)))
	{
		return;
	}
	EnqueuedInputConfigs.Remove(InputConfigHandle);
	InputConfigHandleStack.Remove(InputConfigHandle);
	ensure(InputConfigs.Contains(InputConfigHandle));
	InputConfigs.Remove(InputConfigHandle);
	ScheduleUpdate();
}

TOptional<FGuid> UEstChangeInputConfigSubsystem::PeekInputConfigStack() const
{
	if (!InputConfigHandleStack.Num())
	{
		return {};
	}
	return InputConfigHandleStack.Last();
}

TOptional<FEstInputModeConfig> UEstChangeInputConfigSubsystem::GetCurrentInputConfig() const
{
	if (!CurrentInputConfigHandle.IsSet() || !InputConfigs.Contains(CurrentInputConfigHandle.GetValue()))
	{
		return {};
	}
	return InputConfigs[CurrentInputConfigHandle.GetValue()];
}

void UEstChangeInputConfigSubsystem::ScheduleUpdate()
{
	CICS_LOG(Log, TEXT("ScheduleUpdate"));
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEstChangeInputConfigSubsystem::Update);
}

void UEstChangeInputConfigSubsystem::ApplyInputConfigFromHandle(TOptional<FGuid> InputConfigHandle)
{
	auto const LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;
	auto const PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!IsValid(PC)) return;
	if (CurrentInputConfigHandle == InputConfigHandle) { return; } // already applied
	auto InputConfig = GetInputConfig(InputConfigHandle);
	CICS_LOG(
		Log,
		TEXT("Changing Input Config %s: %s <- %s"),
		InputConfig ? *UEnum::GetDisplayValueAsText(InputConfig->InputMode).ToString() : TEXT("None"),
		*LexToStringOptionalHandle(CurrentInputConfigHandle),
		*LexToStringOptionalHandle(InputConfigHandle)
	);
	CurrentInputConfigHandle = InputConfigHandle;
	if (!InputConfig)
	{
		CICS_LOG(Log, TEXT("Empty Input Config"));
		return;
	}
	// // prevent reporting as enqueued
	// EnqueuedInputConfigs.Remove(InputConfigHandle.GetValue());
	switch (InputConfig->InputMode)
	{
		case EEstInputMode::GameOnly:
		{
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC, InputConfig->bFlushInput);
			auto const GameViewportClient = LocalPlayer->ViewportClient;
			GameViewportClient->SetMouseLockMode(EMouseLockMode::LockOnCapture);
			GameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
			GameViewportClient->SetHideCursorDuringCapture(true);
			PC->SetShowMouseCursor(false);
			break;
		}
		case EEstInputMode::UIOnly:
		{
			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC, InputConfig->WidgetToFocus, InputConfig->MouseLockMode, InputConfig->bFlushInput);
			if (InputConfig->bSetMouseCursor)
			{
				PC->CurrentMouseCursor = static_cast<EMouseCursor::Type>(InputConfig->MouseCursor);
			}
			PC->SetShowMouseCursor(InputConfig->bShowMouseCursor);
			break;
		}
		case EEstInputMode::GameAndUI:
		{
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, InputConfig->WidgetToFocus, InputConfig->MouseLockMode, InputConfig->bHideCursorDuringCapture);
			if (InputConfig->bSetMouseCursor)
			{
				PC->CurrentMouseCursor = static_cast<EMouseCursor::Type>(InputConfig->MouseCursor);
			}
			PC->SetShowMouseCursor(InputConfig->bShowMouseCursor);
			break;
		}
		default:
		{
			CICS_LOG(Error, TEXT("Unknown InputMode %s"), *UEnum::GetDisplayValueAsText(InputConfig->InputMode).ToString());
		}
	}
}

void UEstChangeInputConfigSubsystem::Update()
{
	CICS_LOG(Log, TEXT("Update"));
	auto const PreviousInputConfigHandle = CurrentInputConfigHandle;
	CICS_LOG(Log, TEXT("Previous Input Config %s Index %d"), *LexToStringOptionalHandle(PreviousInputConfigHandle), InputConfigHandleStack.IndexOfByKey(PreviousInputConfigHandle));
	CICS_LOG(Log, TEXT("Input Config Stack %s"), *FString::JoinBy(InputConfigHandleStack, TEXT(", "), [](FGuid const& Handle) { return Handle.ToString(); }));
	ApplyInputConfigFromHandle(PeekInputConfigStack());
	ensure(CurrentInputConfigHandle == PeekInputConfigStack());

	CICS_LOG(Log, TEXT("Applied Input Config %s"), *LexToStringOptionalHandle(CurrentInputConfigHandle));
	CICS_LOG(Log, TEXT("Input Config Stack %s"), *FString::JoinBy(InputConfigHandleStack, TEXT(", "), [](FGuid const& Handle) { return Handle.ToString(); }));
	//~ Report input configs that won't be applied, but are enqueued
	{
		if (EnqueuedInputConfigs.Num() > 0)
		{
			CICS_LOG(Log, TEXT("Enqueued %d Input Configs"), EnqueuedInputConfigs.Num());
		}
		ensure(CurrentInputConfigHandle.IsSet() || EnqueuedInputConfigs.Num() == 0); // shouldn't have enqueued configs if we're not applying anything
		auto LastEnqueuedInputConfigs = EnqueuedInputConfigs;
		EnqueuedInputConfigs.Reset();
		for (auto const InputConfigHandle : LastEnqueuedInputConfigs)
		{
			CICS_LOG(Log, TEXT("Enqueued Input Config %s"), *InputConfigHandle.ToString());
			ensure(GetInputConfig(InputConfigHandle).IsSet());
			OnInputConfigEnqueued.Broadcast(InputConfigHandle);
		}
	}
}
