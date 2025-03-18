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

#include "Templates/TypeHash.h"

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

	friend uint32 GetTypeHash(FCTRLIgnoreInputConfig const& Arg)
	{
		uint32 Hash = HashCombine(GetTypeHash(Arg.bOverrideIgnoreMoveInput), GetTypeHash(Arg.bIgnoreMoveInput));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bOverrideIgnoreLookInput));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bIgnoreLookInput));
		return Hash;
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

	// Whether to flush input. Default is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool bFlushInput = true;

	// Whether to override the default cursor settings for the input mode. Default is false
	// e.g. if false, won't show cursor (or cursor options) for GameOnly mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideInputModeDefault = false;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShowMouseCursor && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	bool CenterMouseOnSet = false;

	// Hardware mouse cursor to use. Requires bShowMouseCursor
	// Used in GameAndUI & UIOnly input modes unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShowMouseCursor && bOverrideMouseCursor && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	ECTRLMouseCursor MouseCursor = ECTRLMouseCursor::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly"))
	bool bOverrideMouseLock = false;

	// Mouse lock mode to use. Default is LockAlways
	// Used in GameAndUI and UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="bOverrideMouseLock && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	EMouseLockMode MouseLockMode = EMouseLockMode::LockAlways;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly"))
	bool bOverrideMouseCapture = false;

	// Mouse capture mode to use. Default is CapturePermanently
	// Used in GameAndUI and UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="bOverrideMouseCapture && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	EMouseCaptureMode MouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	// Whether to hide cursor during capture. Default is false
	// Note option is not normally available for UIOnly input mode, but we expose it here
	// Used in GameAndUI and UIOnly input modes, unless bOverrideInputModeDefault is true
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="bOverrideMouseCapture && (bOverrideInputModeDefault || InputMode != ECTRLInputMode::GameOnly)"))
	bool bHideCursorDuringCapture = false;

	// Optional Widget to focus on mode change. Default is nullptr.
	// Used in GameAndUI & UIOnly input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="InputMode != ECTRLInputMode::GameOnly"))
	TObjectPtr<UUserWidget> WidgetToFocus = nullptr;

	friend bool operator==(FCTRLInputModeConfig const& Lhs, FCTRLInputModeConfig const& RHS)
	{
		return Lhs.InputMode == RHS.InputMode
			&& Lhs.bFlushInput == RHS.bFlushInput
			&& Lhs.bOverrideInputModeDefault == RHS.bOverrideInputModeDefault
			&& Lhs.IgnoreInputConfig == RHS.IgnoreInputConfig
			&& Lhs.bShowMouseCursor == RHS.bShowMouseCursor
			&& Lhs.bOverrideMouseCursor == RHS.bOverrideMouseCursor
			&& Lhs.MouseCursor == RHS.MouseCursor
			&& Lhs.CenterMouseOnSet == RHS.CenterMouseOnSet
			&& Lhs.bOverrideMouseCapture == RHS.bOverrideMouseCapture
			&& Lhs.bOverrideMouseLock == RHS.bOverrideMouseLock
			&& Lhs.MouseLockMode == RHS.MouseLockMode
			&& Lhs.MouseCaptureMode == RHS.MouseCaptureMode
			&& Lhs.bHideCursorDuringCapture == RHS.bHideCursorDuringCapture
			&& Lhs.WidgetToFocus == RHS.WidgetToFocus;
	}

	friend bool operator!=(FCTRLInputModeConfig const& Lhs, FCTRLInputModeConfig const& RHS) { return !(Lhs == RHS); }

	friend uint32 GetTypeHash(FCTRLInputModeConfig const& Arg)
	{
		uint32 Hash = HashCombine(GetTypeHash(Arg.InputMode), GetTypeHash(Arg.bFlushInput));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bOverrideInputModeDefault));
		Hash = HashCombine(Hash, GetTypeHash(Arg.IgnoreInputConfig));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bShowMouseCursor));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bOverrideMouseCursor));
		Hash = HashCombine(Hash, GetTypeHash(Arg.MouseCursor));
		Hash = HashCombine(Hash, GetTypeHash(Arg.CenterMouseOnSet));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bOverrideMouseCapture));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bOverrideMouseLock));
		Hash = HashCombine(Hash, GetTypeHash(Arg.MouseLockMode));
		Hash = HashCombine(Hash, GetTypeHash(Arg.MouseCaptureMode));
		Hash = HashCombine(Hash, GetTypeHash(Arg.bHideCursorDuringCapture));
		Hash = HashCombine(Hash, GetTypeHash(Arg.WidgetToFocus));
		return Hash;
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT(
				"FCTRLInputModeConfig({\nInputMode=%s,\r\tbOverrideInputModeDefault=%s,\r\tbFlushInput=%s,\r\tIgnoreInputConfig=%s,\r\tbShowMouseCursor=%s,\r\tbOverrideMouseCursor=%s,\r\tMouseCursor=%s,\r\tbOverrideMouseLock=%s,\r\tMouseLockMode=%s,\r\tbOverrideMouseCapture=%s,\r\tMouseCaptureMode=%s,\r\tbHideCursorDuringCapture=%s,\r\tWidgetToFocus=%s\r})"
			),
			*UEnum::GetValueAsString(InputMode),
			*LexToString(bOverrideInputModeDefault),
			*LexToString(bFlushInput),
			*IgnoreInputConfig.ToString(),
			*LexToString(bShowMouseCursor),
			*LexToString(bOverrideMouseCursor),
			*UEnum::GetValueAsString(MouseCursor),
			*LexToString(bOverrideMouseLock),
			*UEnum::GetValueAsString(MouseLockMode),
			*LexToString(bOverrideMouseCapture),
			*UEnum::GetValueAsString(MouseCaptureMode),
			*LexToString(bHideCursorDuringCapture),
			*GetNameSafe(WidgetToFocus)
		);
	}

	bool SetsShowMouseCursor() const
	{
		if (!bOverrideInputModeDefault) return false;
		return true;
	}

	bool ShowMouseCursor() const
	{
		if (!bOverrideInputModeDefault) return false;
		return bShowMouseCursor;
	}

	bool OverridesMouseCursor() const
	{
		if (!bOverrideInputModeDefault) return false;
		return bOverrideMouseCursor;
	}

	bool HideCursorDuringCapture() const
	{
		if (!bOverrideInputModeDefault) return false;
		return bHideCursorDuringCapture;
	}

	bool IgnoreMoveInput() const
	{
		if (!bOverrideInputModeDefault) return false;
		return IgnoreInputConfig.IgnoreMoveInput();
	}

	bool OverrideIgnoreMoveInput() const
	{
		if (!bOverrideInputModeDefault) return false;
		return IgnoreInputConfig.bOverrideIgnoreMoveInput;
	}

	bool IgnoreLookInput() const
	{
		return OverridesIgnoreLookInput() && IgnoreInputConfig.IgnoreLookInput();
	}

	bool OverridesIgnoreLookInput() const
	{
		if (!bOverrideInputModeDefault) return false;
		return IgnoreInputConfig.bOverrideIgnoreLookInput;
	}

	bool OverrideMouseCapture() const
	{
		if (!bOverrideInputModeDefault) return false;
		return bOverrideMouseCapture;
	}

	bool OverrideMouseLock() const
	{
		if (!bOverrideInputModeDefault) return false;
		return bOverrideMouseLock;
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
	FGuid GetNextGuid(FCTRLInputModeConfig const& InputConfig) const;

	virtual void Deinitialize() override;

	// Builds the input config from the stack
	// e.g.
	// if the stack is {A, B, C}
	// if A & C set a Mouse Cursor, but B does not, when C is removed, A's MouseCursor is inherited
	// (otherwise, the cursor set in C would stay)
	FCTRLInputModeConfig GetInputConfigFromStack() const;

protected:
	FTimerHandle ScheduledUpdateHandle;
	void ScheduleUpdate();
	void Update();
	void ApplyInputConfigFromHandle(TOptional<FGuid> InputConfigHandle);
	void InternalSetMouseCursor(FCTRLInputModeConfig const& InputConfig) const;
};
