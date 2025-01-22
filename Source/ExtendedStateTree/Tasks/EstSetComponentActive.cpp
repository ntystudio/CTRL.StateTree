// SPDX-FileCopyrightText: 2025 NTY.studio

#include "ExtendedStateTree/Tasks/EstSetComponentActive.h"

#include "StateTreePropertyBindings.h"

#include "ExtendedStateTree/EstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstSetComponentActive)

EStateTreeRunStatus FEstComponentActive::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.ActorComponent) { return EStateTreeRunStatus::Failed; }
	InstanceData.ActorComponent->SetActive(InstanceData.bTargetActiveState, InstanceData.bResetOnEnter);
	return EStateTreeRunStatus::Running;
}

void FEstComponentActive::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		InstanceData.ActorComponent->SetActive(!InstanceData.bTargetActiveState, InstanceData.bResetOnExit);
	}
}

#if WITH_EDITOR
FText FEstComponentActive::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Component Active</s> ");
	FText const ComponentName = EST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, ActorComponent, GetNameSafe(Data->ActorComponent));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetActiveState, FString(Data->bTargetActiveState ? TEXT("Active") : TEXT("Inactive")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetActiveState)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetActiveState, FString(!Data->bTargetActiveState ? TEXT("Active") : TEXT("Inactive"))).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UEstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *ComponentName.ToString(), *UEstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UEstUtils::FormatDescription(Out, Formatting);
}
#endif
