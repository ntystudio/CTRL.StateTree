// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLStateTree/Tasks/CTRLAssignWidgetComponentTask.h"

#include "Blueprint/UserWidget.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include "Components/WidgetComponent.h"

EStateTreeRunStatus FCTRLAssignWidgetComponentTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto const& [Widget, WidgetComponent] = Context.GetInstanceData<FInstanceDataType>(*this);
	
	if (!IsValid(WidgetComponent))
	{
		CTRLST_LOG(Error, TEXT("WidgetComponent is invalid %s"), *GetNameSafe(WidgetComponent));
		return EStateTreeRunStatus::Failed;
	}
	if (!IsValid(Widget))
	{
		CTRLST_LOG(Error, TEXT("Widget is invalid %s"), *GetNameSafe(Widget));
		return EStateTreeRunStatus::Failed;
	}
	if (WidgetComponent->GetWidget() == Widget)
	{
		// Widget is already assigned
		return EStateTreeRunStatus::Running;
	}
	// Assign the widget to the component.
	WidgetComponent->SetWidget(Widget);
	return EStateTreeRunStatus::Running;
}

void FCTRLAssignWidgetComponentTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto const& [Widget, WidgetComponent] = Context.GetInstanceData<FInstanceDataType>(*this);
	if (IsValid(WidgetComponent))
	{
		// Only remove the widget if it was assigned by this task
		if (WidgetComponent->GetWidget() == Widget)
		{
			WidgetComponent->SetWidget(nullptr);
		}
	}
	Super::ExitState(Context, Transition);
}

FText FCTRLAssignWidgetComponentTask::GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	FString Out = TEXT("<s>Assign Widget to Component for </s> ");
	FText const ObjectName = CTRLST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, WidgetComponent, GetNameSafe(Data->WidgetComponent));
	Out = Out.Append(ObjectName.ToString());
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
