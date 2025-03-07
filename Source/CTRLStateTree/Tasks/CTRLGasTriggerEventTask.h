// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "CTRLGasTriggerEventTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLGasTriggerEventTaskData
{
	GENERATED_BODY()

	// not used if event has a target bound
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<AActor>> TargetActors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseEnterGameplayEvent = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bUseEnterGameplayEvent"))
	FGameplayEventData EnterGameplayEvent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(InlineEditConditionToggle))
	bool bUseExitGameplayEvent = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bUseExitGameplayEvent"))
	FGameplayEventData ExitGameplayEvent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bFailIfNotSent = false;
};

USTRUCT(BlueprintType, DisplayName="Trigger GAS Event [CTRL]", meta=(Category="GAS", Keywords="Gameplay"))
struct CTRLSTATETREE_API FCTRLGasTriggerEventTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FCTRLGasTriggerEventTaskData;
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
		return FName("EditorStyle|ClassIcon.K2Node_Event");
	}
#endif

protected:
	[[maybe_unused]] bool SendGameplayEvent(FGameplayEventData const& EventData, TArray<AActor*> const& TargetActors) const;
};
