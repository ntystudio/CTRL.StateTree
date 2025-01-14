#include "EstActivateWidgetTask.h"

#include "CommonActivatableWidget.h"
#include "StateTreeExecutionContext.h"

#include "ExtendedStateTree/EstUtils.h"
#include "ExtendedStateTree/ExtendedStateTree.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstActivateWidgetTask)

EStateTreeRunStatus FEstActivateWidgetTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& Data = Context.GetInstanceData<FEstActivateWidgetTaskData>(*this);
	Data.ActivatableWidget = Cast<UCommonActivatableWidget>(Data.Widget);
	if (!IsValid(Data.ActivatableWidget))
	{
		EST_LOG(Error, TEXT("[FEstActivateWidgetTask] Widget is invalid."));
		return EStateTreeRunStatus::Failed;
	}
	SetWidgetActivationState(Data.ActivatableWidget, Data.bTargetActivationState);
	return EStateTreeRunStatus::Running;
}

void FEstActivateWidgetTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& Data = Context.GetInstanceData<FEstActivateWidgetTaskData>(*this);
	if (IsValid(Data.ActivatableWidget))
	{
		if (Data.bInvertTargetActivationStateOnExit)
		{
			SetWidgetActivationState(Data.ActivatableWidget, !Data.bTargetActivationState);
		}
	}
	Data.Widget = nullptr;
	Data.ActivatableWidget = nullptr;
}

EStateTreeRunStatus FEstActivateWidgetTask::Tick(FStateTreeExecutionContext& Context, float DeltaTime) const
{
	Super::Tick(Context, DeltaTime);
	auto& Data = Context.GetInstanceData<FEstActivateWidgetTaskData>(*this);
	if (IsValid(Data.ActivatableWidget) && Data.bTargetActivationState == Data.ActivatableWidget->IsActivated())
	{
		if (Data.bCompleteOnMatchingActivationState)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Running;
}

void FEstActivateWidgetTask::SetWidgetActivationState(UCommonActivatableWidget* ActivatableWidget, bool const bNewActivationState)
{
	if (!IsValid(ActivatableWidget)) { return; }
	if (bNewActivationState == ActivatableWidget->IsActivated()) { return; }

	if (bNewActivationState)
	{
		ActivatableWidget->ActivateWidget();
	}
	else
	{
		ActivatableWidget->DeactivateWidget();
	}
}
#if WITH_EDITOR
FText FEstActivateWidgetTask::GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	FInstanceDataType const* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	TArray Result = {FString::Printf(TEXT("%s<s>ActivateWidget</s>"), *UEstUtils::GetEndStateSymbol(InstanceData->bCompleteOnMatchingActivationState))};
	FText const WidgetBindingText = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Widget)), Formatting);
	Result.Add(FString::Printf(TEXT("<b>%s</b>"), *WidgetBindingText.ToString()));
	Result.Add(FString::Printf(TEXT("%s<b>%s</b>"), *UEstUtils::SymbolStateEnter, InstanceData->bTargetActivationState ? TEXT("Activate") : TEXT("Deactivate")));
	if (InstanceData->bInvertTargetActivationStateOnExit)
	{
		Result.Add(FString::Printf(TEXT("%s<b>%s</b>"), *UEstUtils::SymbolStateExit, !InstanceData->bTargetActivationState ? TEXT("Activate") : TEXT("Deactivate")));
	}
	return UEstUtils::FormatDescription(FString::Join(Result, TEXT(" ")), Formatting);
}
#endif