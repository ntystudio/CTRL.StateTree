// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "UObject/Object.h"

#include "UseStateTree/Tasks/UstStateTreeTaskCommonBase.h"

#include "UstForwardGameplayEventToStateTreeTask.generated.h"

// Task needs a UObject to be able to bind to ASC dynamic multicast delegates
UCLASS(Hidden)
class USESTATETREE_API UUstEventBridge : public UObject
{
	GENERATED_BODY()

public:
	void GameplayEventCallback(FGameplayEventData const* GameplayEventData) const;
	void GameplayEventContainerCallback(FGameplayTag GameplayTag, FGameplayEventData const* GameplayEventData) const;

	DECLARE_DELEGATE_OneParam(FGameplayEventReceivedDelegate, FGameplayEventData);
	FGameplayEventReceivedDelegate EventReceived;
};

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FUstForwardGameplayEventToStateTreeData
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
	TObjectPtr<UUstEventBridge> Bridge;

	FDelegateHandle BridgeDelegateHandle;
};

/*
 * Re-emits Ability System gameplay events from target actor as StateTree events.
 * Useful to set as a global task.
 */
USTRUCT(BlueprintType, DisplayName="Gameplay → StateTree Event [UST]", meta=(Category="Events"))
struct USESTATETREE_API FUstForwardGameplayEventToStateTreeTask : public FUstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FUstForwardGameplayEventToStateTreeData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual EDataValidationResult Compile(FStateTreeDataView InstanceDataView, TArray<FText>& ValidationMessages) override;

protected:
	UUstEventBridge* MakeListener(FStateTreeExecutionContext const& Context) const;

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
