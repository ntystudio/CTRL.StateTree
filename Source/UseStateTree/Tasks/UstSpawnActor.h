// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Templates/SubclassOf.h"

#include "UseStateTree/Tasks/UstStateTreeTaskCommonBase.h"

#include "UstSpawnActor.generated.h"

enum class ESpawnActorCollisionHandlingMethod : uint8;

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FUstSpawnActorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AActor> ActorClass = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDestroyOnExit = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SpawnLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator SpawnRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPreserveRatio))
	FVector SpawnScale = FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ClampMin="0", UIMin="0", UIMax="360", ClampMax="360", Delta="5", Units="degrees"))
	int32 RandomYaw = 360;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ESpawnActorCollisionHandlingMethod CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Optional))
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Optional))
	TObjectPtr<APawn> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Output")
	TObjectPtr<AActor> SpawnedActor = nullptr;
};

USTRUCT(BlueprintType, DisplayName="Spawn Actor [UST]", meta=(Category="Actor"))
struct USESTATETREE_API FUstSpawnActor : public FUstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FUstSpawnActorData;
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
		return FName("EditorStyle|GraphEditor.SpawnActor_16x");
	}

#endif
};
