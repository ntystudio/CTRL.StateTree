// SPDX-FileCopyrightText: 2025 NTY.studio

#include "ExtendedStateTree/Tasks/EEstSpawnActor.h"

#include "StateTreeExecutionContext.h"

#include "Engine/World.h"

#include "ExtendedStateTree/ExtendedStateTree.h"

#include "Kismet/KismetMathLibrary.h"

EStateTreeRunStatus FEstSpawnActor::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.SpawnedActor) {
		return EStateTreeRunStatus::Running;
	}
	if (!InstanceData.ActorClass)
	{
		EST_LOG(Error, TEXT("Failed to spawn actor: No Actor Class."));
		return EStateTreeRunStatus::Failed;
	}
	auto World = Context.GetWorld();
	if (!World)
	{
		EST_LOG(Error, TEXT("Failed to spawn actor: No World."));
		return EStateTreeRunStatus::Failed;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = InstanceData.Owner;
	SpawnParameters.Instigator = InstanceData.Instigator;
	SpawnParameters.SpawnCollisionHandlingOverride = InstanceData.CollisionHandlingMethod;
	FTransform Transform = FTransform(InstanceData.SpawnRotation, InstanceData.SpawnLocation, InstanceData.SpawnScale);
	auto Rotation = Transform.Rotator();
	Rotation.Yaw += UKismetMathLibrary::RandomIntegerInRange(-InstanceData.RandomYaw * 0.5, InstanceData.RandomYaw * 0.5);
	Transform.SetRotation(Rotation.Quaternion());
	auto SpawnedActor = World->SpawnActor(InstanceData.ActorClass, &Transform, SpawnParameters);
	if (!SpawnedActor)
	{
		EST_LOG(Error, TEXT("Failed to spawn actor: %s"), *InstanceData.ActorClass.Get()->GetDisplayNameText().ToString())
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.SpawnedActor = SpawnedActor;
	return EStateTreeRunStatus::Running;
}

void FEstSpawnActor::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bDestroyOnExit && IsValid(InstanceData.SpawnedActor))
	{
		auto const SpawnedActor = InstanceData.SpawnedActor;
		InstanceData.SpawnedActor = nullptr;
		SpawnedActor->Destroy();
	}
}

FText FEstSpawnActor::GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FEstStateTreeTaskCommonBase::GetDescription(ID, InstanceDataView, BindingLookup, Formatting);
}
