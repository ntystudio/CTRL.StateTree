// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"

#include "Engine/EngineBaseTypes.h"

#include "GenericPlatform/ICursor.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "Subsystems/LocalPlayerSubsystem.h"

#include "EstChangeInputConfigSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEstChangeInputConfigSubsystem, Log, All);

/*
 * Input modes that can be set up for a player controller.
 * Adapted from UWidgetBlueprintLibrary.h
 */
UENUM(BlueprintType)
enum class EEstInputMode : uint8
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
enum class EEstMouseCursor: uint8
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
class UEstMouseCursorUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Convert from EEstMouseCursor to EMouseCursor
	static EMouseCursor::Type ToMouseCursor(EEstMouseCursor const CursorType)
	{
		return EMouseCursor::Type(static_cast<uint8>(CursorType));
	}
};

/*
 * Player Input Mode + Associated Config
 */
USTRUCT(BlueprintType)
struct FEstInputModeConfig
{
	GENERATED_BODY()

	// Input mode to use. Default is GameOnly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEstInputMode InputMode = EEstInputMode::GameOnly;

	// Whether to flush input. Default is true
	// Used in GameOnly and GameAndUI input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="InputMode != EEstInputMode::GameAndUI"))
	bool bFlushInput = true;

	// Whether to hide cursor during capture. Default is false
	// Used in GameAndUI input mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="InputMode == EEstInputMode::GameAndUI"))
	bool bHideCursorDuringCapture = false;

	// Mouse lock mode to use. Default is LockAlways
	// Used in GameAndUI and UIOnly input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="InputMode != EEstInputMode::GameOnly"))
	EMouseLockMode MouseLockMode = EMouseLockMode::LockAlways;

	// Whether to show mouse cursor. Default is true
	// Used in UIOnly and GameAndUI input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bSetMouseCursor && InputMode != EEstInputMode::GameOnly"))
	bool bShowMouseCursor = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShowMouseCursor && InputMode != EEstInputMode::GameOnly"))
	bool bSetMouseCursor = true;

	// Hardware mouse cursor to use. Default is Default
	// Used in UIOnly and GameAndUI input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bShowMouseCursor && bSetMouseCursor && InputMode != EEstInputMode::GameOnly"))
	EEstMouseCursor MouseCursor = EEstMouseCursor::Default;

	// Widget to focus on mode change. Default is nullptr. Optional.
	// Used in UIOnly and GameAndUI input modes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(EditCondition="InputMode != EEstInputMode::GameOnly"))
	TObjectPtr<UUserWidget> WidgetToFocus = nullptr;

	friend bool operator==(FEstInputModeConfig const& Lhs, FEstInputModeConfig const& RHS)
	{
		return Lhs.InputMode == RHS.InputMode
			&& Lhs.bFlushInput == RHS.bFlushInput
			&& Lhs.MouseLockMode == RHS.MouseLockMode
			&& Lhs.bHideCursorDuringCapture == RHS.bHideCursorDuringCapture
			&& Lhs.WidgetToFocus == RHS.WidgetToFocus
			&& Lhs.bSetMouseCursor == RHS.bSetMouseCursor
			&& Lhs.MouseCursor == RHS.MouseCursor
			&& Lhs.bShowMouseCursor == RHS.bShowMouseCursor;
	}

	friend bool operator!=(FEstInputModeConfig const& Lhs, FEstInputModeConfig const& RHS) { return !(Lhs == RHS); }
};

/*
 * Manages a stack of input configs for a player controller.
 */
UCLASS(BlueprintType, DisplayName="Input Config Subsystem [EST]")
class EXTENDEDSTATETREE_API UEstChangeInputConfigSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static UEstChangeInputConfigSubsystem* Get(ULocalPlayer const* LocalPlayer);
	static UEstChangeInputConfigSubsystem* Get(APlayerController const* PC);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FGuid> InputConfigHandleStack;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TMap<FGuid, FEstInputModeConfig> InputConfigs;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TOptional<FGuid> CurrentInputConfigHandle;

	// InputConfigs that were added this tick. Report them as enqueued
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient)
	TSet<FGuid> EnqueuedInputConfigs;

	DECLARE_MULTICAST_DELEGATE_OneParam(FInputConfigChangedDelegate, TOptional<FGuid>);
	FInputConfigChangedDelegate OnInputConfigChanged;

	DECLARE_MULTICAST_DELEGATE_OneParam(FInputConfigEnqueuedDelegate, FGuid);
	FInputConfigEnqueuedDelegate OnInputConfigEnqueued;

public:
	// Push the specified input config onto the stack. Queues an update.
	FGuid PushInputConfig(FEstInputModeConfig const& InputConfig);

	// Remove the specified input config from the stack. Not really a pop. Queues an update.
	void PopInputConfig(FGuid const& InputConfigHandle);

	TOptional<FEstInputModeConfig> GetCurrentInputConfig() const;

	TOptional<FEstInputModeConfig> GetInputConfig(TOptional<FGuid> const& InputConfigHandle) const;

	// Peek at the top of the input config stack
	TOptional<FGuid> PeekInputConfigStack() const;

	FString DescribeHandle(TOptional<FGuid> InputConfigHandle) const;

	virtual void Deinitialize() override;

protected:
	void ScheduleUpdate();
	void Update();
	void ApplyInputConfigFromHandle(TOptional<FGuid> InputConfigHandle);
	FTimerHandle UpdateHandle;
};
