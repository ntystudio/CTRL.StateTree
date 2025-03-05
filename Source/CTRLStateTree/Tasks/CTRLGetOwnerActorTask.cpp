// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLGetOwnerActorTask.h"

#include "StateTreePropertyBindings.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLGetOwnerActorTask)

EStateTreeRunStatus FCTRLGetOwnerActorTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [Object, OwnerActor, bIsValid] = Context.GetInstanceData(*this);
	ON_SCOPE_EXIT
	{
		bIsValid = IsValid(OwnerActor);
	};

	if (!Object) { return EStateTreeRunStatus::Failed; }
	if (auto const Component = Cast<UActorComponent>(Object))
	{
		OwnerActor = Component->GetOwner();
		return EStateTreeRunStatus::Running;
	}
	if (auto const Actor = Cast<AActor>(Object))
	{
		OwnerActor = Actor->GetOwner();
		return EStateTreeRunStatus::Running;
	}

	OwnerActor = Object->GetTypedOuter<AActor>();
	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FCTRLGetOwnerActorTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Get Owner Actor for</s> ");
	FText const ObjectName = CTRLST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, Object, Data->Object ? Data->Object.GetName() : FString("None"));
	Out = Out.Append(ObjectName.ToString());
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
#endif