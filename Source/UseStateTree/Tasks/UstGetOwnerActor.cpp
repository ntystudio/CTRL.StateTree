// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UstGetOwnerActor.h"

#include "StateTreePropertyBindings.h"

#include "GameFramework/Actor.h"

#include "UseStateTree/UstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UstGetOwnerActor)

EStateTreeRunStatus FUstGetOwnerActor::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
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
FText FUstGetOwnerActor::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Get Owner Actor for</s> ");
	FText const ObjectName = UST_GET_BINDING_TEXT(ID,  InstanceDataView, BindingLookup, Formatting, Object, Data->Object ? Data->Object.GetName() : FString("None"));
	Out = Out.Append(ObjectName.ToString());
	return UUstUtils::FormatDescription(Out, Formatting);
}
#endif