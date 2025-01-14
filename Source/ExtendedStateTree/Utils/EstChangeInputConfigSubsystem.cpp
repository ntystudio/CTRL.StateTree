// Fill out your copyright notice in the Description page of Project Settings.

#include "EstChangeInputConfigSubsystem.h"

#include "TimerManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

#include "ExtendedStateTree/ExtendedStateTree.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstChangeInputConfigSubsystem)

DEFINE_LOG_CATEGORY(LogEstChangeInputConfigSubsystem)

#define CICS_LOG(Verbosity, ...) UE_LOG(LogEstChangeInputConfigSubsystem, Verbosity, ##__VA_ARGS__)
#define CICS_CLOG(Cond, Verbosity, ...) UE_CLOG(Cond, LogEstChangeInputConfigSubsystem, Verbosity, ##__VA_ARGS__)

FString LexToStringOptionalHandle(TOptional<FGuid> const& Guid)
{
	return Guid.IsSet() ? Guid.GetValue().ToString() : TEXT("None");
}

UEstChangeInputConfigSubsystem* UEstChangeInputConfigSubsystem::Get(ULocalPlayer const* LocalPlayer)
{
	return IsValid(LocalPlayer) ? LocalPlayer->GetSubsystem<UEstChangeInputConfigSubsystem>() : nullptr;
}

UEstChangeInputConfigSubsystem* UEstChangeInputConfigSubsystem::Get(APlayerController const* PC)
{
	return Get(IsValid(PC) ? PC->GetLocalPlayer() : nullptr);
}

void UEstChangeInputConfigSubsystem::Deinitialize()
{
	if (auto const World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}
	InputConfigHandleStack.Empty();
	InputConfigs.Empty();
	CurrentInputConfigHandle.Reset();
	OnInputConfigEnqueued.Clear();
	OnInputConfigChanged.Clear();
	EnqueuedInputConfigs.Empty();
	Super::Deinitialize();
}

TOptional<FEstInputModeConfig> UEstChangeInputConfigSubsystem::GetInputConfig(TOptional<FGuid> const& InputConfigHandle) const
{
	if (!InputConfigHandle.IsSet()) return {};
	FGuid const Key = InputConfigHandle.GetValue();
	if (!InputConfigs.Contains(Key))
	{
		ensure(!InputConfigHandleStack.Contains(Key));
		ensure(!EnqueuedInputConfigs.Contains(Key));
		return {};
	}
	return InputConfigs[Key];
}

FString UEstChangeInputConfigSubsystem::DescribeHandle(TOptional<FGuid> InputConfigHandle) const
{
	auto const InputConfig = GetInputConfig(InputConfigHandle);
	if (!InputConfig.IsSet()) return TEXT("None");
	return UEnum::GetDisplayValueAsText(InputConfig->InputMode).ToString();
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
	if (!ensure(InputConfigHandleStack.Contains(InputConfigHandle))) return;
	EnqueuedInputConfigs.Remove(InputConfigHandle);
	InputConfigHandleStack.Remove(InputConfigHandle);
	ensure(InputConfigs.Contains(InputConfigHandle));
	InputConfigs.Remove(InputConfigHandle);
	ScheduleUpdate();
}

TOptional<FGuid> UEstChangeInputConfigSubsystem::PeekInputConfigStack() const
{
	if (!InputConfigHandleStack.Num()) return {};
	return InputConfigHandleStack.Last();
}

TOptional<FEstInputModeConfig> UEstChangeInputConfigSubsystem::GetCurrentInputConfig() const
{
	if (!CurrentInputConfigHandle.IsSet() || !InputConfigs.Contains(CurrentInputConfigHandle.GetValue())) return {};
	return InputConfigs[CurrentInputConfigHandle.GetValue()];
}

void UEstChangeInputConfigSubsystem::ScheduleUpdate()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(UpdateHandle)) return;
	UpdateHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEstChangeInputConfigSubsystem::Update);
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
		Verbose,
		TEXT("ApplyInputConfigFromHandle: Changing Input Config: %s <- %s"),
		*DescribeHandle(InputConfigHandle),
		*DescribeHandle(CurrentInputConfigHandle)
	);
	CurrentInputConfigHandle = InputConfigHandle;
	if (!InputConfig)
	{
		CICS_LOG(Log, TEXT("ApplyInputConfigFromHandle: Empty Input Config"));
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
			CICS_LOG(Error, TEXT("ApplyInputConfigFromHandle: Unknown InputMode %s"), *UEnum::GetDisplayValueAsText(InputConfig->InputMode).ToString());
		}
	}
}

void UEstChangeInputConfigSubsystem::Update()
{
	auto const PreviousInputConfigHandle = CurrentInputConfigHandle;
	ApplyInputConfigFromHandle(PeekInputConfigStack());
	ensure(CurrentInputConfigHandle == PeekInputConfigStack());

	ensure(CurrentInputConfigHandle.IsSet() || EnqueuedInputConfigs.Num() == 0); // shouldn't have enqueued configs if we're not applying anything
	auto CurrentEnqueuedInputConfigs = EnqueuedInputConfigs;
	EnqueuedInputConfigs.Reset();
	for (auto const InputConfigHandle : CurrentEnqueuedInputConfigs)
	{
		ensure(GetInputConfig(InputConfigHandle).IsSet());
		OnInputConfigEnqueued.Broadcast(InputConfigHandle);
	}
}
