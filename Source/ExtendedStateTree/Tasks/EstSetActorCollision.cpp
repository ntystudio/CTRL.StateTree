// SPDX-FileCopyrightText: 2025 NTY.studio

#include "ExtendedStateTree/Tasks/EstSetActorCollision.h"

#include "StateTreePropertyBindings.h"

#include "ExtendedStateTree/EstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstSetActorCollision)

EStateTreeRunStatus FEstSetActorCollision::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.Actor) { return EStateTreeRunStatus::Failed; }
	InstanceData.Actor->SetActorEnableCollision(InstanceData.bTargetCollisionEnabled);
	return EStateTreeRunStatus::Running;
}

void FEstSetActorCollision::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType const& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRevertOnExit)
	{
		InstanceData.Actor->SetActorEnableCollision(!InstanceData.bTargetCollisionEnabled);
	}
}

#if WITH_EDITOR
FText FEstSetActorCollision::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Set Actor Collision</s> ");
	FText const ActorName = EST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, Actor, GetNameSafe(Data->Actor));
	auto const& Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const TargetStateText = EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetCollisionEnabled, Data->bTargetCollisionEnabled ? TEXT("Enabled") : TEXT("Disabled"));
	auto const Source = BindingLookup.GetPropertyBindingSource(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bTargetCollisionEnabled)));
	FString const NotTargetStateString = FString::Printf(
		TEXT("%s%s"),
		Source ? TEXT("!") : TEXT(""),
		*EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, bTargetCollisionEnabled, !Data->bTargetCollisionEnabled ? TEXT("Enabled") : TEXT("Disabled")).ToString()
	);
	FString const ExitString = Data->bRevertOnExit ? FString::Printf(TEXT("%s<s>:</s> %s"), *UEstUtils::SymbolStateExit, *NotTargetStateString) : TEXT("");
	Out = Out.Append(FString::Printf(TEXT("%s<s>:</s> %s %s %s"), *ActorName.ToString(), *UEstUtils::SymbolStateEnter, *TargetStateText.ToString(), *ExitString)).TrimStartAndEnd();
	return UEstUtils::FormatDescription(Out, Formatting);
}
#endif
