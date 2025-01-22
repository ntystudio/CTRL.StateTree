// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "Templates/SubclassOf.h"

#include "EEstSpawnActor.generated.h"

enum class ESpawnActorCollisionHandlingMethod : uint8;

USTRUCT(BlueprintType)
struct FEstSpawnActorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SpawnLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator SpawnRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SpawnScale = FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 RandomYaw = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethod;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<AActor> Owner;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<APawn> Instigator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDestroyOnExit = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Output")
	TObjectPtr<AActor> SpawnedActor;
};

USTRUCT(BlueprintType, DisplayName="Spawn Actor [EST]")
struct EXTENDEDSTATETREE_API FEstSpawnActor : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FEstSpawnActorData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(
		FGuid const& ID,
		FStateTreeDataView InstanceDataView,
		IStateTreeBindingLookup const& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text
	) const override;

	virtual FName GetIconName() const override
	{
		return FName("GraphEditor.SpawnActor_16x");
	}

#endif
};
