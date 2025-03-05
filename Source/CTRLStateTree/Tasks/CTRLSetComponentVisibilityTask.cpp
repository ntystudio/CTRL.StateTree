// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLStateTree/Tasks/CTRLSetComponentVisibility.h"

#include "StateTreePropertyBindings.h"

#include "Components/SceneComponent.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLSetComponentVisibility)

EStateTreeRunStatus FCTRLComponentVisibility::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.SceneComponent) { return EStateTreeRunStatus::Failed; }
	SetTargetVisibility(Context, InstanceData.bTargetVisibility);
	return EStateTreeRunStatus::Running;
}

void FCTRLComponentVisibility::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		SetTargetVisibility(Context, !InstanceData.bTargetVisibility);
	}
}

void FCTRLComponentVisibility::SetTargetVisibility(FStateTreeExecutionContext& Context, bool const bNewVisibility) const
{
	FInstanceDataType const& Data = Context.GetInstanceData(*this);

	if (!Data.SceneComponent) { return; }
	Data.SceneComponent->SetVisibility(bNewVisibility, Data.bPropagateToChildren);
}

#if WITH_EDITOR
FText FCTRLComponentVisibility::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Component Visibility</s> ");
	FText const SceneComponentName = CTRLST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, SceneComponent, Data->SceneComponent ? Data->SceneComponent.GetName() : FString("None"));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, FString(Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetVisibility)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, !Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UCTRLStateTreeUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *SceneComponentName.ToString(), *UCTRLStateTreeUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
#endif

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━ Component Visibility In-Game ━━━━━━━━━━━━━━━━━━━━━━━━━━ //

void FCTRLComponentVisibilityInGame::SetTargetVisibility(FStateTreeExecutionContext& Context, bool const bNewVisibility) const
{
	FInstanceDataType const& Data = Context.GetInstanceData(*this);

	if (!Data.SceneComponent) { return; }
	Data.SceneComponent->SetHiddenInGame(!bNewVisibility, Data.bPropagateToChildren);
}

#if WITH_EDITOR
FText FCTRLComponentVisibilityInGame::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	FString Out = TEXT("<s>Set Component Visibility In-Game</s> ");
	FText const SceneComponentName = CTRLST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, SceneComponent, GetNameSafe(Data->SceneComponent));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, FString(Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetVisibility)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, !Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UCTRLStateTreeUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *SceneComponentName.ToString(), *UCTRLStateTreeUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
#endif
