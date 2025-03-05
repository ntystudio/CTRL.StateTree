// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLSetActorCollisionTask.h"

#include "StateTreePropertyBindings.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLSetActorCollisionTask)

EStateTreeRunStatus FCTRLSetActorCollisionTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Actor) { return EStateTreeRunStatus::Failed; }
	InstanceData.Actor->SetActorEnableCollision(InstanceData.bTargetCollisionEnabled);
	return EStateTreeRunStatus::Running;
}

void FCTRLSetActorCollisionTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		InstanceData.Actor->SetActorEnableCollision(!InstanceData.bTargetCollisionEnabled);
	}
}

#if WITH_EDITOR
FText FCTRLSetActorCollisionTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Actor Collision</s> ");
	FText const ActorName = CTRLST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, Actor, GetNameSafe(Data->Actor));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetCollisionEnabled, Data->bTargetCollisionEnabled ? TEXT("Enabled") : TEXT("Disabled"));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetCollisionEnabled)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*CTRLST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetCollisionEnabled, !Data->bTargetCollisionEnabled ? TEXT("Enabled") : TEXT("Disabled")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UCTRLStateTreeUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *ActorName.ToString(), *UCTRLStateTreeUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
#endif
