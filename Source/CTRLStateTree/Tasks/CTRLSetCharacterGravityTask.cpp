// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLSetCharacterGravityTask.h"

#include "StateTreePropertyBindings.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLSetCharacterGravityTask)

EStateTreeRunStatus FCTRLSetCharacterGravityTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Character) { return EStateTreeRunStatus::Failed; }
	InstanceData.PreviousGravityScale = InstanceData.Character->GetCharacterMovement()->GravityScale;
	InstanceData.Character->GetCharacterMovement()->GravityScale = InstanceData.TargetGravityScale;
	return EStateTreeRunStatus::Running;
}

void FCTRLSetCharacterGravityTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		InstanceData.Character->GetCharacterMovement()->GravityScale = InstanceData.PreviousGravityScale;
	}
}

#if WITH_EDITOR
FText FCTRLSetCharacterGravityTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Actor Gravity Scale</s> ");
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const CharacterName = CTRLST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, Character, GetNameSafe(Data->Character));
	FText const TargetStateText = CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, TargetGravityScale, FString::Printf(TEXT("%2.2f"), Data->TargetGravityScale));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, TargetGravityScale)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, TargetGravityScale, TEXT("Revert")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UCTRLStateTreeUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *CharacterName.ToString(), *UCTRLStateTreeUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
#endif
