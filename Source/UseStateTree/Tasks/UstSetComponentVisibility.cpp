// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UseStateTree/Tasks/UstSetComponentVisibility.h"

#include "StateTreePropertyBindings.h"

#include "Components/SceneComponent.h"

#include "UseStateTree/UstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UstSetComponentVisibility)

EStateTreeRunStatus FUstComponentVisibility::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.SceneComponent) { return EStateTreeRunStatus::Failed; }
	SetTargetVisibility(Context, InstanceData.bTargetVisibility);
	return EStateTreeRunStatus::Running;
}

void FUstComponentVisibility::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		SetTargetVisibility(Context, !InstanceData.bTargetVisibility);
	}
}

void FUstComponentVisibility::SetTargetVisibility(FStateTreeExecutionContext& Context, bool const bNewVisibility) const
{
	FInstanceDataType const& Data = Context.GetInstanceData(*this);

	if (!Data.SceneComponent) { return; }
	Data.SceneComponent->SetVisibility(bNewVisibility, Data.bPropagateToChildren);
}

#if WITH_EDITOR
FText FUstComponentVisibility::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Component Visibility</s> ");
	FText const SceneComponentName = UST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, SceneComponent, Data->SceneComponent ? Data->SceneComponent.GetName() : FString("None"));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, FString(Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetVisibility)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, !Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UUstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *SceneComponentName.ToString(), *UUstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UUstUtils::FormatDescription(Out, Formatting);
}
#endif

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━ Component Visibility In-Game ━━━━━━━━━━━━━━━━━━━━━━━━━━ //

void FUstComponentVisibilityInGame::SetTargetVisibility(FStateTreeExecutionContext& Context, bool const bNewVisibility) const
{
	FInstanceDataType const& Data = Context.GetInstanceData(*this);

	if (!Data.SceneComponent) { return; }
	Data.SceneComponent->SetHiddenInGame(!bNewVisibility, Data.bPropagateToChildren);
}

#if WITH_EDITOR
FText FUstComponentVisibilityInGame::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	FString Out = TEXT("<s>Set Component Visibility In-Game</s> ");
	FText const SceneComponentName = UST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, SceneComponent, GetNameSafe(Data->SceneComponent));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, FString(Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetVisibility)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetVisibility, !Data->bTargetVisibility ? TEXT("Visible") : TEXT("Hidden")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UUstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *SceneComponentName.ToString(), *UUstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UUstUtils::FormatDescription(Out, Formatting);
}
#endif
