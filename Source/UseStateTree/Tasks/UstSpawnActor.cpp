// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UseStateTree/Tasks/UstSpawnActor.h"

#include "StateTreeExecutionContext.h"

#include "Engine/World.h"

#include "Kismet/KismetMathLibrary.h"

#include "UseStateTree/UseStateTree.h"
#include "UseStateTree/UstUtils.h"

EStateTreeRunStatus FUstSpawnActor::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.SpawnedActor)
	{
		return EStateTreeRunStatus::Running;
	}
	if (!InstanceData.ActorClass)
	{
		UST_LOG(Error, TEXT("Failed to spawn actor: No Actor Class."));
		return EStateTreeRunStatus::Failed;
	}
	auto const World = Context.GetWorld();
	if (!World)
	{
		UST_LOG(Error, TEXT("Failed to spawn actor: No World."));
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
		UST_LOG(Error, TEXT("Failed to spawn actor: %s"), *InstanceData.ActorClass.Get()->GetDisplayNameText().ToString())
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.SpawnedActor = SpawnedActor;
	return EStateTreeRunStatus::Running;
}

void FUstSpawnActor::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bDestroyOnExit && IsValid(InstanceData.SpawnedActor))
	{
		auto const SpawnedActor = InstanceData.SpawnedActor;
		InstanceData.SpawnedActor = nullptr;
		SpawnedActor->Destroy();
	}
}
#if WITH_EDITOR
FText FUstSpawnActor::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = FString::Printf(TEXT("<s>Spawn Actor</s> %s "), *UUstUtils::SymbolStateEnter);
	FInstanceDataType const* Data = InstanceDataView.GetPtr<FInstanceDataType>();
	FText const ActorClassName = UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, ActorClass, Data->ActorClass ? Data->ActorClass.Get()->GetDisplayNameText().ToString() : FString("None"));
	auto const DestroyOnExitPath = FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, bDestroyOnExit));
	auto const DestroyOnExitSource = BindingLookup.GetPropertyBindingSource(DestroyOnExitPath);
	FText const DestroyOnExit = DestroyOnExitSource ? BindingLookup.GetBindingSourceDisplayName(DestroyOnExitPath, Formatting) : FText::GetEmpty();
	FString const OnExitString = (DestroyOnExitSource || Data->bDestroyOnExit) ? FString::Printf(TEXT(" %s Destroy %s"), *UUstUtils::SymbolStateExit, *DestroyOnExit.ToString()) : FString();
	FText const LocationText = UST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, SpawnLocation, Data->SpawnLocation.ToCompactString());
	Out = Out.Append(FString::Printf(TEXT("%s <s>at</s> %s %s"), *ActorClassName.ToString(), *LocationText.ToString(), *OnExitString));
	return UUstUtils::FormatDescription(Out, Formatting);
}
#endif
