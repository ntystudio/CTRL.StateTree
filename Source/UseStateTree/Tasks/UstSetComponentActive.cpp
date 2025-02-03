// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UseStateTree/Tasks/UstSetComponentActive.h"

#include "StateTreePropertyBindings.h"

#include "UseStateTree/UstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UstSetComponentActive)

EStateTreeRunStatus FUstComponentActive::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.ActorComponent) { return EStateTreeRunStatus::Failed; }
	InstanceData.ActorComponent->SetActive(InstanceData.bTargetActiveState, InstanceData.bResetOnEnter);
	return EStateTreeRunStatus::Running;
}

void FUstComponentActive::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		InstanceData.ActorComponent->SetActive(!InstanceData.bTargetActiveState, InstanceData.bResetOnExit);
	}
}

#if WITH_EDITOR
FText FUstComponentActive::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Component Active</s> ");
	FText const ComponentName = UST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, ActorComponent, GetNameSafe(Data->ActorComponent));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetActiveState, FString(Data->bTargetActiveState ? TEXT("Active") : TEXT("Inactive")));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetActiveState)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetActiveState, FString(!Data->bTargetActiveState ? TEXT("Active") : TEXT("Inactive"))).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UUstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *ComponentName.ToString(), *UUstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UUstUtils::FormatDescription(Out, Formatting);
}
#endif
