// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "CTRLStateTreeCommonBaseTask.h"

#include "Runtime/Engine/Public/Subsystems/WorldSubsystem.h"

#include "CTRLCreateWidgetTask.generated.h"

struct FStateTreeInstanceStorage;
class APlayerController;
struct FStateTreeTransitionResult;
struct FStateTreeExecutionContext;
class UUserWidget;
class UCommonUserWidget;
class UCommonActivatableWidget;

UCLASS(BlueprintType, Blueprintable, meta=(Hidden, Category="Internal"))
class UCTRLCreateWidgetTaskData : public UObject
{
	GENERATED_BODY()

public:
	UCTRLCreateWidgetTaskData(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Context")
	TObjectPtr<APlayerController> TargetPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TObjectPtr<UUserWidget> WidgetTemplate;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Output", SkipSerialization, DuplicateTransient)
	TObjectPtr<UUserWidget> Widget = nullptr;

	// for binding convenience, if passed-in widget is a common widget, this will be set
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Output", SkipSerialization, DuplicateTransient)
	TObjectPtr<UCommonUserWidget> CommonWidget;

	// for binding convenience, if passed-in widget is activatable, this will be set
	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Output", SkipSerialization, DuplicateTransient)
	TObjectPtr<UCommonActivatableWidget> ActivatableWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAddToViewportOnEnter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRemoveFromViewportOnExit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bAddToViewportOnEnter", EditConditionHides))
	int32 ZIndex = 0;
	DECLARE_MULTICAST_DELEGATE(FOnTaskDataDestruct);
	FOnTaskDataDestruct OnDestruct;
	void Destruct();
	bool Setup();
	void SendEvent(FStateTreeEvent const& Event) const;

	void SetCachedInstanceDataFromContext(FStateTreeExecutionContext const& Context) const;

	void ClearCachedInstanceData() const;
	/** Cached instance data while the node is active. */
	mutable TWeakPtr<FStateTreeInstanceStorage> WeakInstanceStorage;

	/** Cached owner while the node is active. */
	UPROPERTY()
	mutable TObjectPtr<UObject> CachedOwner = nullptr;

	/** Cached State Tree of owning execution frame. */
	UPROPERTY()
	mutable TObjectPtr<UStateTree const> CachedFrameStateTree = nullptr;

	/** Cached root state of owning execution frame. */
	mutable FStateTreeStateHandle CachedFrameRootState;

	/** Cached State where the node is processed on. */
	mutable FStateTreeStateHandle CachedState;
};

USTRUCT(BlueprintType, Blueprintable, DisplayName = "Create Widget Task [CTRL]", meta=(Category="UI"))
struct CTRLSTATETREE_API FCTRLCreateWidgetTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

public:
	using UInstanceDataType = UCTRLCreateWidgetTaskData;

	virtual UStruct const* GetInstanceDataType() const override { return UInstanceDataType::StaticClass(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(
		FGuid const& ID,
		FStateTreeDataView InstanceDataView,
		IStateTreeBindingLookup const& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text
	) const override;

	virtual FName GetIconName() const override
	{
		return FName("CoreStyle|Icons.Success");
	}

#endif
};

class UStateTreeNodeBlueprintBase;
/**
 * World subsystem for supporting sending state tree events from any object e.g. from widgets.
 * Associate an object with a state tree node via Listen() to support static function SendStateTreeEvent.
 */
UCLASS(ClassGroup=(CTRL), DisplayName="StateTree Send Event Subsystem [CTRL]")
class CTRLSTATETREE_API UCTRLStateTreeWidgetEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// mapping between object + node
	// to support static function SendStateTreeEvent
	TMap<TWeakObjectPtr<UUserWidget>, TWeakObjectPtr<UCTRLCreateWidgetTaskData>> WidgetToNodeMap;

	static UCTRLStateTreeWidgetEventSubsystem* Get(UObject const* WorldContextObject);
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="Widget"), DisplayName="Send StateTree Event")
	static bool K2_SendStateTreeEvent(UUserWidget* Widget, FStateTreeEvent const& StateTreeEvent);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="Widget"))
	bool SendStateTreeEvent(UUserWidget* Widget, FStateTreeEvent const& StateTreeEvent);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="Widget"))
	void Listen(UUserWidget* Widget, UCTRLCreateWidgetTaskData* Node);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="Widget"))
	void Unlisten(UUserWidget const* Widget);

	void PurgeStale();
};
