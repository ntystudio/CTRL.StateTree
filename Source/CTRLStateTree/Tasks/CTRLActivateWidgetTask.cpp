#include "CTRLActivateWidgetTask.h"

#include "CommonActivatableWidget.h"
#include "StateTreeExecutionContext.h"

#include "CTRLStateTree/CTRLStateTree.h"
#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLActivateWidgetTask)

EStateTreeRunStatus FCTRLActivateWidgetTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& Data = Context.GetInstanceData<FCTRLActivateWidgetTaskData>(*this);
	Data.ActivatableWidget = Cast<UCommonActivatableWidget>(Data.Widget);
	if (!IsValid(Data.ActivatableWidget))
	{
		CTRLST_LOG(Error, TEXT("[FCTRLActivateWidgetTask] Widget is invalid."));
		return EStateTreeRunStatus::Failed;
	}
	SetWidgetActivationState(Data.ActivatableWidget, Data.bTargetActivationState);
	return EStateTreeRunStatus::Running;
}

void FCTRLActivateWidgetTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& Data = Context.GetInstanceData<FCTRLActivateWidgetTaskData>(*this);
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

EStateTreeRunStatus FCTRLActivateWidgetTask::Tick(FStateTreeExecutionContext& Context, float const DeltaTime) const
{
	Super::Tick(Context, DeltaTime);
	auto& Data = Context.GetInstanceData<FCTRLActivateWidgetTaskData>(*this);
	if (IsValid(Data.ActivatableWidget) && Data.bTargetActivationState == Data.ActivatableWidget->IsActivated())
	{
		if (Data.bCompleteOnMatchingActivationState)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Running;
}

void FCTRLActivateWidgetTask::SetWidgetActivationState(UCommonActivatableWidget* ActivatableWidget, bool const bNewActivationState)
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
FText FCTRLActivateWidgetTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FInstanceDataType const* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	TArray Result = {FString::Printf(TEXT("%s<s>ActivateWidget</s>"), *UCTRLStateTreeUtils::GetEndStateSymbol(InstanceData->bCompleteOnMatchingActivationState))};
	FText const WidgetBindingText = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Widget)), Formatting);
	if (WidgetBindingText.IsEmpty())
	{
		Result.Add(FString::Printf(TEXT("<b>%s Invalid Widget</b>"), *UCTRLStateTreeUtils::SymbolInvalid));
	}
	else
	{
		Result.Add(FString::Printf(TEXT("<b>%s</b>"), *WidgetBindingText.ToString()));
	}
	Result.Add(FString::Printf(TEXT("%s<b>%s</b>"), *UCTRLStateTreeUtils::SymbolStateEnter, InstanceData->bTargetActivationState ? TEXT("Activate") : TEXT("Deactivate")));
	if (InstanceData->bInvertTargetActivationStateOnExit)
	{
		Result.Add(FString::Printf(TEXT("%s<b>%s</b>"), *UCTRLStateTreeUtils::SymbolStateExit, !InstanceData->bTargetActivationState ? TEXT("Activate") : TEXT("Deactivate")));
	}
	return UCTRLStateTreeUtils::FormatDescription(FString::Join(Result, TEXT(" ")), Formatting);
}
#endif
