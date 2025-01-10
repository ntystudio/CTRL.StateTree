// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "UObject/Object.h"

#include "EstForwardGameplayEventToStateTreeTask.generated.h"

// Task needs a UObject to be able to bind to ASC dynamic multicast delegates
UCLASS(Hidden)
class EXTENDEDSTATETREE_API UEstEventBridge : public UObject
{
	GENERATED_BODY()

public:
	void GameplayEventCallback(FGameplayEventData const* GameplayEventData) const;
	void GameplayEventContainerCallback(FGameplayTag GameplayTag, FGameplayEventData const* GameplayEventData) const;

	DECLARE_DELEGATE_OneParam(FGameplayEventReceivedDelegate, FGameplayEventData);
	FGameplayEventReceivedDelegate EventReceived;
};

USTRUCT(BlueprintType, meta=(HiddenByDefault))
struct FEstForwardGameplayEventToStateTreeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Context")
	TObjectPtr<AActor> Actor;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bOnlyMatchExact = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	bool bOnlyTriggerOnce = false;

	UPROPERTY(Transient)
	TObjectPtr<UEstEventBridge> Bridge;

	FDelegateHandle BridgeDelegateHandle;
};

/*
 * Re-emits Ability System gameplay events from target actor as StateTree events.
 * Useful to set as a global task.
 */
USTRUCT(BlueprintType, DisplayName="Gameplay -> StateTree Event (new) [EST]", meta=(Category="Events"))
struct EXTENDEDSTATETREE_API FEstForwardGameplayEventToStateTreeTask : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:

	using FInstanceDataType = FEstForwardGameplayEventToStateTreeData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual EDataValidationResult Compile(FStateTreeDataView InstanceDataView, TArray<FText>& ValidationMessages) override;


#if WITH_EDITOR
	virtual FText GetDescription(
		FGuid const& ID,
		FStateTreeDataView InstanceDataView,
		IStateTreeBindingLookup const& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text
	) const override;

	virtual FName GetIconName() const override
	{
		return FName("EnhancedInputEditor|ClassIcon.InputAction");
	}

	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Grey;
	}
#endif

protected:
	UEstEventBridge* MakeListener(FStateTreeExecutionContext const& Context) const;
};

