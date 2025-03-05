// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"

#include "Engine/EngineBaseTypes.h"
#include "Engine/TimerHandle.h"

#include "GenericPlatform/ICursor.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Subsystems/LocalPlayerSubsystem.h"

#include "CTRLChangeInputConfigSubsystem.generated.h"

class UUserWidget;
DECLARE_LOG_CATEGORY_EXTERN(LogCTRLChangeInputConfigSubsystem, Log, All);

/*
 * Input modes that can be set up for a player controller.
 * Adapted from UWidgetBlueprintLibrary.h
 */
UENUM(BlueprintType)
enum class ECTRLInputMode : uint8
{
	/**
	 * Set up an input mode that allows only player input / player controller to respond to user input.
	 */
	GameOnly,
	/**
	 * Set up an input mode that allows only the UI to respond to user input, and if the UI doesn't handle it player input / player controller gets a chance.
	 * 
	 * Note: This means that any bound Input events in the widget will be called.
	 */
	GameAndUI,
	/**
    	 * Set up an input mode that allows only the UI to respond to user input.
    	 */
	UIOnly
};

/*
 * Mouse Cursor types that can be set for a player controller.
 * Adapted from ICursor.h to be BlueprintType
 */
UENUM(BlueprintType)
enum class ECTRLMouseCursor: uint8
{
	/** Causes no mouse cursor to be visible */
	None,

	/** Default cursor (arrow) */
	Default,

	/** Text edit beam */
	TextEditBeam,

	/** Resize horizontal */
	ResizeLeftRight,

	/** Resize vertical */
	ResizeUpDown,

	/** Resize diagonal */
	ResizeSouthEast,

	/** Resize other diagonal */
	ResizeSouthWest,

	/** MoveItem */
	CardinalCross,

	/** Target Cross */
	Crosshairs,

	/** Hand cursor */
	Hand,

	/** Grab Hand cursor */
	GrabHand,

	/** Grab Hand cursor closed */
	GrabHandClosed,

	/** a circle with a diagonal line through it */
	SlashedCircle,

	/** Eye-dropper cursor for picking colors */
	EyeDropper,

	/** Custom cursor shape for platforms that support setting a native cursor shape. Same as specifying None if not set. */
	Custom,

	/** Number of cursors we support */
	TotalCursorCount
};

UCLASS()
class CTRLSTATETREE_API UCTRLMouseCursorUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Convert from ECTRLMouseCursor to EMouseCursor
	static EMouseCursor::Type ToMouseCursor(ECTRLMouseCursor const CursorType)
	{
		return static_cast<EMouseCursor::Type>(static_cast<uint8>(CursorType));
	}
};

USTRUCT(BlueprintType)
struct CTRLSTATETREE_API FCTRLIgnoreInputConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	bool bOverrideIgnoreMoveInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideIgnoreMoveInput"))
	bool bIgnoreMoveInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	bool bOverrideIgnoreLookInput = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideIgnoreLookInput"))
	bool bIgnoreLookInput = false;

	bool IgnoreLookInput() const
	{
		return bOverrideIgnoreLookInput && bIgnoreLookInput;
	}

	bool IgnoreMoveInput() const
	{
		return bOverrideIgnoreMoveInput && bIgnoreMoveInput;
	}

	friend bool operator==(FCTRLIgnoreInputConfig const& Lhs, FCTRLIgnoreInputConfig const& RHS)
	{
		return Lhs.bOverrideIgnoreMoveInput == RHS.bOverrideIgnoreMoveInput
			&& Lhs.bIgnoreMoveInput == RHS.bIgnoreMoveInput
			&& Lhs.bOverrideIgnoreLookInput == RHS.bOverrideIgnoreLookInput
			&& Lhs.bIgnoreLookInput == RHS.bIgnoreLookInput;
	}

	friend bool operator!=(FCTRLIgnoreInputConfig const& Lhs, FCTRLIgnoreInputConfig const& RHS) { return !(Lhs == RHS); }

	FString ToString() const
	{
		return FString::Printf(
			TEXT("FCTRLIgnoreInputConfig(bOverrideIgnoreMoveInput=%s, bIgnoreMoveInput=%s, bOverrideIgnoreLookInput=%s, bIgnoreLookInput=%s)"),
			*LexToString(bOverrideIgnoreMoveInput),
			*LexToString(bIgnoreMoveInput),
			*LexToString(bOverrideIgnoreLookInput),
			*LexToString(bIgnoreLookInput)
		);
	}
};

/*
 * Player Input Mode + Associated Config
 */
USTRUCT(BlueprintType)
struct CTRLSTATETREE_API FCTRLInputModeConfig
{
	GENERATED_BODY()

	// Input mode to use. Default is GameOnly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECTRLInputMode InputMode = ECTRLInputMode::GameOnly;

	// Whether to override the default cursor settings for the input mode. Default is false
	// e.g. if false, won't show cursor (or cursor options) for GameOnly mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideInputModeDefault = false;

	// Whether to flush input. Default is true
	// Used in GameOnly and GameAndUI input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="InputMode != ECTRLInputMode::GameAndUI"))
	bool bFlushInput = true;

	// Whether to ignore Player look/move input
	// Only used if InputMode is not UIOnly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="Ignore Input", meta=(EditCondition="InputMode != ECTRLInputMode::UIOnly", ShowOnlyInnerProperties))
	FCTRLIgnoreInputConfig IgnoreInputConfig;

	// Whether to show mouse cursor. Default is true
	// Used in GameAndUI & UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly"))
	bool bShowMouseCursor = true;

	// Whether to set a mouse cursor. Default is true. Requires bShowMouseCursor
	// Used in GameAndUI & UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShowMouseCursor && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	bool bOverrideMouseCursor = false;

	// Hardware mouse cursor to use. Requires bShowMouseCursor
	// Used in GameAndUI & UIOnly input modes unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShowMouseCursor && bOverrideMouseCursor && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	ECTRLMouseCursor MouseCursor = ECTRLMouseCursor::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly"))
	bool bOverrideMouseCaptureLock = false;

	// Mouse lock mode to use. Default is LockAlways
	// Used in GameAndUI and UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="bOverrideMouseCaptureLock && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	EMouseLockMode MouseLockMode = EMouseLockMode::LockAlways;

	// Mouse capture mode to use. Default is CapturePermanently
	// Used in GameAndUI and UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="bOverrideMouseCaptureLock && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	EMouseCaptureMode MouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	// Whether to hide cursor during capture. Default is false
	// Note option is not normally available for UIOnly input mode, but we expose it here
	// Used in GameAndUI and UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="bOverrideMouseCaptureLock && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	bool bHideCursorDuringCapture = false;

	// Optional Widget to focus on mode change. Default is nullptr.
	// Used in GameAndUI & UIOnly input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="InputMode != ECTRLInputMode::GameOnly"))
	TObjectPtr<UUserWidget> WidgetToFocus = nullptr;

	friend bool operator==(FCTRLInputModeConfig const& Lhs, FCTRLInputModeConfig const& RHS)
	{
		return Lhs.InputMode == RHS.InputMode
			&& Lhs.bOverrideInputModeDefault == RHS.bOverrideInputModeDefault
			&& Lhs.bFlushInput == RHS.bFlushInput
			&& Lhs.IgnoreInputConfig == RHS.IgnoreInputConfig
			&& Lhs.bShowMouseCursor == RHS.bShowMouseCursor
			&& Lhs.bOverrideMouseCursor == RHS.bOverrideMouseCursor
			&& Lhs.MouseCursor == RHS.MouseCursor
			&& Lhs.bOverrideMouseCaptureLock == RHS.bOverrideMouseCaptureLock
			&& Lhs.MouseLockMode == RHS.MouseLockMode
			&& Lhs.MouseCaptureMode == RHS.MouseCaptureMode
			&& Lhs.bHideCursorDuringCapture == RHS.bHideCursorDuringCapture
			&& Lhs.WidgetToFocus == RHS.WidgetToFocus;
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT(
				"FCTRLInputModeConfig({\nInputMode=%s,\r\tbOverrideInputModeDefault=%s,\r\tbFlushInput=%s,\r\tIgnoreInputConfig=%s,\r\tbShowMouseCursor=%s,\r\tbOverrideMouseCursor=%s,\r\tMouseCursor=%s,\r\tbOverrideMouseCaptureLock=%s,\r\tMouseLockMode=%s,\r\tMouseCaptureMode=%s,\r\tbHideCursorDuringCapture=%s,\r\tWidgetToFocus=%s\r})"
			),
			*UEnum::GetValueAsString(InputMode),
			*LexToString(bOverrideInputModeDefault),
			*LexToString(bFlushInput),
			*IgnoreInputConfig.ToString(),
			*LexToString(bShowMouseCursor),
			*LexToString(bOverrideMouseCursor),
			*UEnum::GetValueAsString(MouseCursor),
			*LexToString(bOverrideMouseCaptureLock),
			*UEnum::GetValueAsString(MouseLockMode),
			*UEnum::GetValueAsString(MouseCaptureMode),
			*LexToString(bHideCursorDuringCapture),
			*GetNameSafe(WidgetToFocus)
		);
	}

	friend bool operator!=(FCTRLInputModeConfig const& Lhs, FCTRLInputModeConfig const& RHS) { return !(Lhs == RHS); }

	bool ShowMouseCursor() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::GameOnly)
		{
			return false;
		}
		return bShowMouseCursor;
	}

	bool OverridesMouseCursor() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::GameOnly)
		{
			return false;
		}
		return ShowMouseCursor() && bOverrideMouseCursor;
	}

	bool HideCursorDuringCapture() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::GameOnly)
		{
			return false;
		}
		return bHideCursorDuringCapture;
	}

	bool IgnoreMoveInput() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::UIOnly)
		{
			return false;
		}
		return IgnoreInputConfig.IgnoreMoveInput();
	}

	bool OverrideIgnoreMoveInput() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::UIOnly)
		{
			return false;
		}
		return IgnoreInputConfig.bOverrideIgnoreMoveInput;
	}

	bool IgnoreLookInput() const
	{
		return OverridesIgnoreLookInput() && IgnoreInputConfig.IgnoreLookInput();
	}

	bool OverridesIgnoreLookInput() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::UIOnly)
		{
			return false;
		}
		return IgnoreInputConfig.bOverrideIgnoreLookInput;
	}

	bool OverrideMouseCaptureLock() const
	{
		if (!bOverrideInputModeDefault && InputMode == ECTRLInputMode::GameOnly)
		{
			return false;
		}
		return bOverrideMouseCaptureLock;
	}
};

UCLASS(BlueprintType, Blueprintable, DisplayName="Input Config Preset [CTRL]")
class CTRLSTATETREE_API UCTRLInputConfigPreset : public UDataAsset
{
	GENERATED_BODY()

public:
	UCTRLInputConfigPreset()
	{
		DebugName = GetClass()->GetDisplayNameText().ToString()
		                      .Replace(TEXT("_"), TEXT(" "))
		                      .Replace(TEXT("Input Config Preset"), TEXT(""))
		                      .Replace(TEXT("CTRL"), TEXT(""), ESearchCase::CaseSensitive)
		                      .Replace(TEXT("UST"), TEXT(""), ESearchCase::CaseSensitive);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DebugName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine=true))
	FText DebugDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ShowOnlyInnerProperties))
	FCTRLInputModeConfig InputConfig;
};

/*
 * Manages a stack of input configs for a player controller.
 */
UCLASS(BlueprintType, DisplayName="Input Config Subsystem [CTRL]")
class CTRLSTATETREE_API UCTRLChangeInputConfigSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static UCTRLChangeInputConfigSubsystem* Get(ULocalPlayer const* LocalPlayer);
	static UCTRLChangeInputConfigSubsystem* Get(APlayerController const* PC);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FGuid> InputConfigHandleStack;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FGuid, FCTRLInputModeConfig> InputConfigs;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TOptional<FGuid> CurrentInputConfigHandle;

	// InputConfigs that were added this tick. Report them as enqueued
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient)
	TSet<FGuid> EnqueuedInputConfigs;

	UPROPERTY(BlueprintReadOnly, Transient)
	bool bIsIgnoringMoveInput = false;

	UPROPERTY(BlueprintReadOnly, Transient)
	bool bIsIgnoringLookInput = false;

	DECLARE_MULTICAST_DELEGATE_OneParam(FInputConfigChangedDelegate, TOptional<FGuid>);
	FInputConfigChangedDelegate OnInputConfigChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FInputConfigEnqueuedDelegate, FGuid);
	FInputConfigEnqueuedDelegate OnInputConfigEnqueued;

public:
	// Push the specified input config onto the stack. Queues an update.
	FGuid PushInputConfig(FCTRLInputModeConfig const& InputConfig);

	// Remove the specified input config from the stack. Not really a pop. Queues an update.
	void PopInputConfig(FGuid const& InputConfigHandle);

	TOptional<FCTRLInputModeConfig> GetCurrentInputConfig() const;

	TOptional<FCTRLInputModeConfig> GetInputConfig(TOptional<FGuid> const& InputConfigHandle) const;

	// Peek at the top of the input config stack
	TOptional<FGuid> PeekInputConfigStack() const;

	FString DescribeHandle(TOptional<FGuid> InputConfigHandle) const;

	virtual void Deinitialize() override;

	// builds the input config from the stack
	// e.g.
	// if the stack is {A, B, C}
	// if A & C set a Mouse Cursor, but B does not, when C is removed, A's MouseCursor is inherited
	// (otherwise, the cursor set in C would stay)
	FCTRLInputModeConfig GetInputConfigFromStack() const;

protected:
	void ScheduleUpdate();
	void Update();
	void ApplyInputConfigFromHandle(TOptional<FGuid> InputConfigHandle);
	FTimerHandle UpdateHandle;
};
