#pragma once

#include "CoreMinimal.h"

#include "UseStateTree/Tasks/UstStateTreeTaskCommonBase.h"

#include "UstActivateWidgetTask.generated.h"

class UCommonActivatableWidget;
class UUserWidget;

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FUstActivateWidgetTaskData
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
	bool bCompleteOnMatchingActivationState = false;

	FDelegateHandle OnActivatedHandle;
	FDelegateHandle OnDeactivatedHandle;

	void OnActivationChanged();
};

/**
 * Activates or deactivates a widget based on the target activation state.
 */
USTRUCT(BlueprintType, DisplayName="Activate Widget [UST]", meta=(Category="UI"))
struct FUstActivateWidgetTask : public FUstStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FUstActivateWidgetTask()
	{
		bShouldCallTick = true;
	}

	using FInstanceDataType = FUstActivateWidgetTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) const override;

protected:
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
