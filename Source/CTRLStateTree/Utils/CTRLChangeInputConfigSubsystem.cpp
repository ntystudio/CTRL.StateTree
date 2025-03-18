// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLChangeInputConfigSubsystem.h"

#include "TimerManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "CTRLCore/CTRLUnique.h"

#include "CTRLStateTree/CTRLStateTree.h"

#include "Components/Viewport.h"
#include "Widgets/SViewport.h"

#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

#include "Framework/Application/SlateApplication.h"

#include "Kismet/GameplayStatics.h"

#include "Slate/SceneViewport.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLChangeInputConfigSubsystem)

DEFINE_LOG_CATEGORY(LogCTRLChangeInputConfigSubsystem)

#define CICS_LOG(Verbosity, ...) UE_LOG(LogCTRLChangeInputConfigSubsystem, Verbosity, ##__VA_ARGS__)
#define CICS_CLOG(Cond, Verbosity, ...) UE_CLOG(Cond, LogCTRLChangeInputConfigSubsystem, Verbosity, ##__VA_ARGS__)

FString LexToStringOptionalHandle(TOptional<FGuid> const& Guid)
{
	return Guid.IsSet() ? Guid.GetValue().ToString() : TEXT("None");
}

UCTRLChangeInputConfigSubsystem* UCTRLChangeInputConfigSubsystem::Get(ULocalPlayer const* LocalPlayer)
{
	return IsValid(LocalPlayer) ? LocalPlayer->GetSubsystem<UCTRLChangeInputConfigSubsystem>() : nullptr;
}

UCTRLChangeInputConfigSubsystem* UCTRLChangeInputConfigSubsystem::Get(APlayerController const* PC)
{
	return Get(IsValid(PC) ? PC->GetLocalPlayer() : nullptr);
}

void UCTRLChangeInputConfigSubsystem::Deinitialize()
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

TOptional<FCTRLInputModeConfig> UCTRLChangeInputConfigSubsystem::GetInputConfig(TOptional<FGuid> const& InputConfigHandle) const
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

FString UCTRLChangeInputConfigSubsystem::DescribeHandle(TOptional<FGuid> InputConfigHandle) const
{
	auto const InputConfig = GetInputConfig(InputConfigHandle);
	if (!InputConfig.IsSet()) return TEXT("None");
	auto const ShortId = UCTRLUnique::GetUnambiguousShortId(TSet<FGuid>(InputConfigHandleStack), InputConfigHandle.GetValue());
	auto const Index = InputConfigHandleStack.IndexOfByKey(InputConfigHandle.GetValue());
	return FString::Printf(TEXT("%d:%s %s"), Index, *ShortId, *UEnum::GetDisplayValueAsText(InputConfig->InputMode).ToString());
}

FGuid UCTRLChangeInputConfigSubsystem::GetNextGuid(FCTRLInputModeConfig const& InputConfig) const
{
	auto const Hash = GetTypeHash(InputConfig);
	FBlake3 HashBuilder;
	HashBuilder.Update(&Hash, sizeof(Hash));
	for (auto [Guid, Config] : InputConfigs)
	{
		auto InnerHash = GetTypeHash(Config);
		HashBuilder.Update(&InnerHash, sizeof(InnerHash));
	}
	return FGuid::NewGuidFromHash(HashBuilder.Finalize());
}

FGuid UCTRLChangeInputConfigSubsystem::PushInputConfig(FCTRLInputModeConfig const& InputConfig)
{
	FGuid const InputConfigHandle = GetNextGuid(InputConfig);
	InputConfigHandleStack.Push(InputConfigHandle);
	InputConfigs.Add(InputConfigHandle, InputConfig);
	CICS_LOG(Verbose, TEXT("PushInputConfig: %s %s"), *DescribeHandle(InputConfigHandle), *InputConfigHandle.ToString());
	EnqueuedInputConfigs.Add(InputConfigHandle);
	ScheduleUpdate();
	return InputConfigHandle;
}

void UCTRLChangeInputConfigSubsystem::PopInputConfig(FGuid const& InputConfigHandle)
{
	if (!ensure(InputConfigHandleStack.Contains(InputConfigHandle))) return;
	CICS_LOG(Verbose, TEXT("PopInputConfig: %s "), *DescribeHandle(InputConfigHandle));
	EnqueuedInputConfigs.Remove(InputConfigHandle);
	InputConfigHandleStack.Remove(InputConfigHandle);
	ensure(InputConfigs.Contains(InputConfigHandle));
	InputConfigs.Remove(InputConfigHandle);
	ScheduleUpdate();
}

TOptional<FGuid> UCTRLChangeInputConfigSubsystem::PeekInputConfigStack() const
{
	if (!InputConfigHandleStack.Num()) return {};
	return InputConfigHandleStack.Last();
}

TOptional<FCTRLInputModeConfig> UCTRLChangeInputConfigSubsystem::GetCurrentInputConfig() const
{
	if (!CurrentInputConfigHandle.IsSet() || !InputConfigs.Contains(CurrentInputConfigHandle.GetValue())) return {};
	return InputConfigs[CurrentInputConfigHandle.GetValue()];
}

void UCTRLChangeInputConfigSubsystem::ScheduleUpdate()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ScheduledUpdateHandle)) return;
	ScheduledUpdateHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UCTRLChangeInputConfigSubsystem::Update);
}

FCTRLInputModeConfig UCTRLChangeInputConfigSubsystem::GetInputConfigFromStack() const
{
	TOptional<FCTRLInputModeConfig> InputConfigContext;
	for (auto const InputConfigHandle : InputConfigHandleStack)
	{
		if (!InputConfigHandle.IsValid()) continue;
		auto const InputConfig = GetInputConfig(InputConfigHandle);
		if (!InputConfig.IsSet()) continue;
		if (!InputConfigContext.IsSet())
		{
			InputConfigContext = InputConfig;
			continue;
		}

		InputConfigContext->InputMode = InputConfig->InputMode;
		InputConfigContext->bOverrideInputModeDefault = InputConfig->bOverrideInputModeDefault;
		InputConfigContext->bFlushInput = InputConfig->bFlushInput;
		if (InputConfig->OverridesIgnoreLookInput())
		{
			InputConfigContext->IgnoreInputConfig.bOverrideIgnoreLookInput = true;
			InputConfigContext->IgnoreInputConfig.bIgnoreLookInput = InputConfig->IgnoreLookInput();
		}
		if (InputConfig->OverrideIgnoreMoveInput())
		{
			InputConfigContext->IgnoreInputConfig.bOverrideIgnoreMoveInput = true;
			InputConfigContext->IgnoreInputConfig.bIgnoreMoveInput = InputConfig->IgnoreMoveInput();
		}
		if (InputConfig->SetsShowMouseCursor())
		{
			InputConfigContext->bShowMouseCursor = InputConfig->ShowMouseCursor();
			InputConfigContext->CenterMouseOnSet = InputConfig->CenterMouseOnSet;
		}

		if (InputConfig->OverridesMouseCursor())
		{
			InputConfigContext->bOverrideMouseCursor = true;
			InputConfigContext->MouseCursor = InputConfig->MouseCursor;
		}

		if (InputConfig->OverrideMouseLock())
		{
			InputConfigContext->bOverrideMouseLock = true;
			InputConfigContext->MouseLockMode = InputConfig->MouseLockMode;
		}

		if (InputConfig->OverrideMouseCapture())
		{
			InputConfigContext->bOverrideMouseCapture = true;
			InputConfigContext->MouseCaptureMode = InputConfig->MouseCaptureMode;
			InputConfigContext->bHideCursorDuringCapture = InputConfig->HideCursorDuringCapture();
		}

		if (InputConfig->InputMode != ECTRLInputMode::GameOnly)
		{
			InputConfigContext->WidgetToFocus = InputConfig->WidgetToFocus;
		}
	}
	if (!InputConfigContext.IsSet()) return {};
	return InputConfigContext.GetValue();
}

void UCTRLChangeInputConfigSubsystem::InternalSetMouseCursor(FCTRLInputModeConfig const& InputConfig) const
{
	auto const LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;
	auto const PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!IsValid(PC)) return;
	auto const GameViewportClient = LocalPlayer->ViewportClient;
	GameViewportClient->GetGameViewport()->CaptureMouse(true);
	auto& SlateOperations = LocalPlayer->GetSlateOperations();
	TSharedPtr<SViewport> const ViewportWidget = GameViewportClient->GetGameViewportWidget();
	if (!ViewportWidget.IsValid()) return;
	TSharedRef<SViewport> const ViewportWidgetRef = ViewportWidget.ToSharedRef();
	switch (InputConfig.InputMode)
	{
		case ECTRLInputMode::GameOnly:
		{
			SlateOperations.UseHighPrecisionMouseMovement(ViewportWidgetRef);
			SlateOperations.SetUserFocus(ViewportWidgetRef);
			SlateOperations.LockMouseToWidget(ViewportWidgetRef);
			SlateOperations.CaptureMouse(ViewportWidgetRef);

			break;
		}
		case ECTRLInputMode::UIOnly: {}
		case ECTRLInputMode::GameAndUI:
		{
			bool const bLockMouseToViewport = InputConfig.MouseLockMode == EMouseLockMode::LockAlways
				|| (InputConfig.MouseLockMode == EMouseLockMode::LockInFullscreen && GameViewportClient->IsExclusiveFullscreenViewport());
			if (bLockMouseToViewport)
			{
				SlateOperations.LockMouseToWidget(ViewportWidgetRef);
			}
			else
			{
				SlateOperations.ReleaseMouseLock();
			}
			// GameViewportClient->SetIgnoreInput(false);
			// GameViewportClient->SetHideCursorDuringCapture(InputConfig.bHideCursorDuringCapture);
			// GameViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CaptureDuringMouseDown);
			if (IsValid(InputConfig.WidgetToFocus))
			{
				SlateOperations.SetUserFocus(InputConfig.WidgetToFocus->TakeWidget());
			}
			else
			{
				SlateOperations.SetUserFocus(ViewportWidgetRef);
			}

			break;
		}
	}
	// GameViewportClient->SetMouseLockMode(EMouseLockMode::LockOnCapture);
	// GameViewportClient->SetIgnoreInput(false);
	if (InputConfig.OverridesMouseCursor())
	{
		PC->CurrentMouseCursor = static_cast<EMouseCursor::Type>(InputConfig.MouseCursor);
	}
	if (InputConfig.OverrideMouseLock())
	{
		GameViewportClient->SetMouseLockMode(InputConfig.MouseLockMode);
	}
	if (InputConfig.OverrideMouseCapture())
	{
		GameViewportClient->SetHideCursorDuringCapture(InputConfig.bHideCursorDuringCapture);
		GameViewportClient->SetMouseCaptureMode(InputConfig.MouseCaptureMode);
	}
	if (InputConfig.SetsShowMouseCursor())
	{
		PC->SetShowMouseCursor(InputConfig.ShowMouseCursor());
	}
	if (InputConfig.CenterMouseOnSet)
	{
		auto const ViewportSize = GameViewportClient->Viewport->GetSizeXY();
		PC->SetMouseLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	}
	if (InputConfig.bFlushInput)
	{
		PC->FlushPressedKeys();
	}
}

void UCTRLChangeInputConfigSubsystem::ApplyInputConfigFromHandle(TOptional<FGuid> InputConfigHandle)
{
	auto const LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer) return;
	auto const PC = LocalPlayer->GetPlayerController(GetWorld());
	if (!IsValid(PC)) return;
	if (CurrentInputConfigHandle == InputConfigHandle) { return; } // already applied

	bool const bHasInputConfig = CurrentInputConfigHandle.IsSet();
	CICS_LOG(
		Verbose,
		TEXT("ApplyInputConfigFromHandle: Changing Input Config: %s <- %s"),
		*DescribeHandle(InputConfigHandle),
		*DescribeHandle(CurrentInputConfigHandle)
	);
	CurrentInputConfigHandle = InputConfigHandle;
	if (!GetInputConfig(InputConfigHandle))
	{
		CICS_LOG(Log, TEXT("ApplyInputConfigFromHandle: Empty Input Config"));
	}

	auto const InputConfig = GetInputConfigFromStack();

	CICS_LOG(
		Verbose,
		TEXT("ApplyInputConfigFromHandle: Input Config: %s"),
		*InputConfig.ToString()
	);

	switch (InputConfig.InputMode)
	{
		case ECTRLInputMode::GameOnly:
		{
			if (!InputConfig.bOverrideInputModeDefault)
			{
				UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC, InputConfig.bFlushInput);
				PC->SetShowMouseCursor(false);
			}
			else
			{
				InternalSetMouseCursor(InputConfig);
			}
			break;
		}
		case ECTRLInputMode::GameAndUI:
		{
			if (!InputConfig.bOverrideInputModeDefault)
			{
				UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, InputConfig.WidgetToFocus, InputConfig.MouseLockMode, InputConfig.bHideCursorDuringCapture);
				PC->SetShowMouseCursor(InputConfig.ShowMouseCursor());
			}
			else
			{
				InternalSetMouseCursor(InputConfig);
			}
		}
		case ECTRLInputMode::UIOnly:
		{
			if (!InputConfig.bOverrideInputModeDefault)
			{
				UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PC, InputConfig.WidgetToFocus, InputConfig.MouseLockMode, InputConfig.bFlushInput);
			}
			else
			{
				InternalSetMouseCursor(InputConfig);
			}
			break;
		}
		default:
		{
			CICS_LOG(Error, TEXT("ApplyInputConfigFromHandle: Unknown InputMode %s"), *UEnum::GetDisplayValueAsText(InputConfig.InputMode).ToString());
		}
	}

	if (InputConfig.OverridesIgnoreLookInput())
	{
		bool const bIgnoreLookInput = InputConfig.IgnoreLookInput();
		if (!bHasInputConfig || bIsIgnoringLookInput != bIgnoreLookInput)
		{
			bIsIgnoringLookInput = bIgnoreLookInput;
			PC->SetIgnoreLookInput(bIsIgnoringLookInput);
		}
	}

	if (InputConfig.OverrideIgnoreMoveInput())
	{
		bool const bIgnoreMoveInput = InputConfig.IgnoreMoveInput();
		if (!bHasInputConfig || bIsIgnoringMoveInput != bIgnoreMoveInput)
		{
			bIsIgnoringMoveInput = bIgnoreMoveInput;
			PC->SetIgnoreMoveInput(bIsIgnoringMoveInput);
		}
	}
	if (InputConfig.InputMode == ECTRLInputMode::UIOnly && !InputConfig.OverridesIgnoreLookInput() && !InputConfig.OverrideIgnoreMoveInput())
	{
		auto const GameViewportClient = LocalPlayer->ViewportClient;
		GameViewportClient->SetIgnoreInput(true);
	}
	// prevent reporting as enqueued
	// EnqueuedInputConfigs.Remove(InputConfigHandle.GetValue());
}

void UCTRLChangeInputConfigSubsystem::Update()
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
