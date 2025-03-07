// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "CTRLSetCharacterGravityTask.generated.h"

class ACharacter;

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLSetActorGravityData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Context")
	TObjectPtr<ACharacter> Character = nullptr;

	// e.g.1.0 for 100% gravity, 0.0 for 0% gravity,  0.5 for 50% gravity, etc 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(UIMin="0", UIMax="10", Delta="0.1", Units="Multiplier"))
	float TargetGravityScale = 0.0f;

	// if true, will revert to the previous gravity scale when exiting the state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRevertOnExit = false;

	// stored gravity scale before entering the state
	UPROPERTY(BlueprintReadWrite, Transient)
	float PreviousGravityScale = 1.0f;
};

USTRUCT(BlueprintType, DisplayName="Set Character Gravity Scale [CTRL]", meta=(Category="Actor"))
struct CTRLSTATETREE_API FCTRLSetCharacterGravityTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FCTRLSetActorGravityData;
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
