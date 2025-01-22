// SPDX-FileCopyrightText: 2025 NTY.studio

#include "ExtendedStateTree/Tasks/EstSetComponentVisibility.h"

#include "StateTreePropertyBindings.h"

#include "ExtendedStateTree/EstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstSetComponentVisibility)

EStateTreeRunStatus FEstComponentVisibility::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.SceneComponent) { return EStateTreeRunStatus::Failed; }
	SetTargetVisibility(Context, InstanceData.bTargetVisibility);
	return EStateTreeRunStatus::Running;
}

void FEstComponentVisibility::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		SetTargetVisibility(Context, !InstanceData.bTargetVisibility);
	}
}

void FEstComponentVisibility::SetTargetVisibility(FStateTreeExecutionContext& Context, bool const bNewVisibility) const
{
	FInstanceDataType const& Data = Context.GetInstanceData(*this);

	if (!Data.SceneComponent) { return; }
	Data.SceneComponent->SetVisibility(bNewVisibility, Data.bPropagateToChildren);
}

#if WITH_EDITOR
FText FEstComponentVisibility::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Component Visibility</s> ");
	FText const SceneComponentName = EST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, SceneComponent, Data->SceneComponent ? Data->SceneComponent.GetName() : FString("None"));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, FString(Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetVisibility)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, !Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UEstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *SceneComponentName.ToString(), *UEstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UEstUtils::FormatDescription(Out, Formatting);
}
#endif

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━ Component Visibility In-Game ━━━━━━━━━━━━━━━━━━━━━━━━━━ //

void FEstComponentVisibilityInGame::SetTargetVisibility(FStateTreeExecutionContext& Context, bool const bNewVisibility) const
{
	FInstanceDataType const& Data = Context.GetInstanceData(*this);

	if (!Data.SceneComponent) { return; }
	Data.SceneComponent->SetHiddenInGame(!bNewVisibility, Data.bPropagateToChildren);
}

#if WITH_EDITOR
FText FEstComponentVisibilityInGame::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	FString Out = TEXT("<s>Set Component Visibility In-Game</s> ");
	FText const SceneComponentName = EST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, SceneComponent, GetNameSafe(Data->SceneComponent));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, FString(Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetVisibility)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, !Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UEstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *SceneComponentName.ToString(), *UEstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UEstUtils::FormatDescription(Out, Formatting);
}
#endif
