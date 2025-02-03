// SPDX-FileCopyrightText: 2025 NTY.studio
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "UseStateTree/Tasks/UstStateTreeTaskCommonBase.h"

#include "UstSetCharacterGravity.generated.h"

class ACharacter;

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FUstSetActorGravityData
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

USTRUCT(BlueprintType, DisplayName="Set Character Gravity Scale [UST]", meta=(Category="Actor"))
struct USESTATETREE_API FUstSetCharacterGravity : public FUstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FUstSetActorGravityData;
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