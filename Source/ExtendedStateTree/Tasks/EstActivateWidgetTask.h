#pragma once

#include "CoreMinimal.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "EstActivateWidgetTask.generated.h"

class UCommonActivatableWidget;
class UUserWidget;

USTRUCT(BlueprintType, meta=(Category="UI"))
struct FEstActivateWidgetTaskData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UUserWidget> Widget;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "Output")
	TObjectPtr<UCommonActivatableWidget> ActivatableWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bTargetActivationState = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bInvertTargetActivationStateOnExit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool bCompleteOnMatchingActivationState = true;
	
	FDelegateHandle OnActivatedHandle;
	FDelegateHandle OnDeactivatedHandle;

	void OnActivationChanged();
};
/**
 * Activates or deactivates a widget based on the target activation state.
 */
USTRUCT(BlueprintType, DisplayName="Activate Widget [EST]", meta=(Category="UI"))
struct FEstActivateWidgetTask : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FEstActivateWidgetTask()
	{
		bShouldCallTick = true;
	}
	using FInstanceDataType = FEstActivateWidgetTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) const override;

	static void SetWidgetActivationState(UCommonActivatableWidget* ActivatableWidget, bool bNewActivationState);
#if WITH_EDITOR
	virtual FText GetDescription(
		FGuid const& ID,
		FStateTreeDataView InstanceDataView,
		IStateTreeBindingLookup const& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text
	) const override;

	virtual FName GetIconName() const override
	{
		return FName("WidgetReflectorStyleStyle|WidgetReflector.TabIcon");
	}
#endif
};
