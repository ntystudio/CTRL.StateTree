// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "UObject/Object.h"

#include "CTRLGasEventToStateTreeEventTask.generated.h"

// Task needs a UObject to be able to bind to ASC dynamic multicast delegates
UCLASS(Hidden)
class CTRLSTATETREE_API UCTRLStateTreeEventBridge : public UObject
{
	GENERATED_BODY()

public:
	void GameplayEventCallback(FGameplayEventData const* GameplayEventData) const;
	void GameplayEventContainerCallback(FGameplayTag GameplayTag, FGameplayEventData const* GameplayEventData) const;

	DECLARE_DELEGATE_OneParam(FGameplayEventReceivedDelegate, FGameplayEventData);
	FGameplayEventReceivedDelegate EventReceived;
};

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLGasEventToStateTreeEventTaskData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Context")
	TWeakObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer EventTags;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bOnlyMatchExact = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bOnlyTriggerOnce = false;

	UPROPERTY(Transient)
	TObjectPtr<UCTRLStateTreeEventBridge> Bridge = nullptr;

	TArray<FDelegateHandle> BridgeDelegateHandles;
};

/*
 * Re-emits Ability System gameplay events from target actor as StateTree events.
 * Useful to set as a global task.
 */
USTRUCT(BlueprintType, DisplayName="GAS Event → StateTree Event [CTRL]", meta=(Category="GAS", Keywords="Gameplay"))
struct CTRLSTATETREE_API FCTRLGasEventToStateTreeEventTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FCTRLGasEventToStateTreeEventTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	bool ListenForEvents(FStateTreeExecutionContext const& Context) const;
	void UnlistenForEvents(FStateTreeExecutionContext const& Context) const;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual EDataValidationResult Compile(FStateTreeDataView InstanceDataView, TArray<FText>& ValidationMessages) override;

protected:
	UCTRLStateTreeEventBridge* MakeBridge(FStateTreeExecutionContext const& Context) const;

public:
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
};
