// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "CTRLGetOwnerActorTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLGetOwnerActorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Context")
	TObjectPtr<UObject> Object;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Output")
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Output")
	bool bIsValid = false;
};

USTRUCT(BlueprintType, DisplayName="Get Owner Actor [CTRL]", meta=(Category="Actor"))
struct CTRLSTATETREE_API FCTRLGetOwnerActorTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()
public:
	using FInstanceDataType = FCTRLGetOwnerActorData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;

#if WITH_EDITOR
	virtual FText GetDescription(
		FGuid const& ID,
		FStateTreeDataView InstanceDataView,
		IStateTreeBindingLookup const& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text
	) const override;

	virtual FName GetIconName() const override
	{
		return FName("EditorStyle|Icons.Visibility");
	}

#endif
};