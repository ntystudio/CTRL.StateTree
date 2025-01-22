// SPDX-FileCopyrightText: 2025 NTY.studio
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "EstSetCharacterGravity.generated.h"

class ACharacter;

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FEstSetActorGravityData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Context")
	TObjectPtr<ACharacter> Character = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(UIMin="0", UIMax="10", Delta="0.1", Units="Multiplier"))
	float TargetGravityScale = 0.0f;
	// if true, will revert to the previous gravity scale when exiting the state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRevertOnExit = false;

	UPROPERTY(BlueprintReadWrite, Transient)
	float PreviousGravityScale = 1.0f;
};

USTRUCT(BlueprintType, DisplayName="Set Character Gravity Scale [EST]", meta=(Category="Actor"))
struct EXTENDEDSTATETREE_API FEstSetCharacterGravity : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FEstSetActorGravityData;
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
		return FName("CoreStyle|Icons.Success");
	}

#endif
};