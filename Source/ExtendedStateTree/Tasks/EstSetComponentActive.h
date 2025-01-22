// SPDX-FileCopyrightText: 2025 NTY.studio
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "EstSetComponentActive.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FEstComponentActiveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	TObjectPtr<UActorComponent> ActorComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTargetActiveState = true;

	// If true when exiting the state, the component active state will be set to the opposite of the target state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRevertOnExit = false;

	// If true, the component will be reset to its default state when entering the state and setting to the target state
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bResetOnEnter = false;

	// If true, the component will be reset to its default state when exiting the state and setting to the opposite of the target state
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bRevertOnExit"))
	bool bResetOnExit = false;
};

USTRUCT(BlueprintType, DisplayName="Set Component Active [EST]", meta=(Category="Component", Hidden))
struct EXTENDEDSTATETREE_API FEstComponentActive : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FEstComponentActiveData;
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