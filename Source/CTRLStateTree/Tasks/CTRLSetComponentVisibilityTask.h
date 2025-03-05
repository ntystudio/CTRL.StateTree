// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "CTRLSetComponentVisibilityTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLComponentVisibilityTaskData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
	TObjectPtr<USceneComponent> SceneComponent = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTargetVisibility = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPropagateToChildren = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bRevertOnExit = false;
};

USTRUCT(BlueprintType, DisplayName="Set Component Visibility [CTRL]", meta=(Category="Component"))
struct CTRLSTATETREE_API FCTRLComponentVisibilityTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FCTRLComponentVisibilityTaskData;
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
		return FName("EditorStyle|Icons.Visibility");
	}

#endif

protected:
	[[maybe_unused]] virtual void SetTargetVisibility(FStateTreeExecutionContext& Context, bool bNewVisibility) const;
};

USTRUCT(DisplayName="Set Component Visibility In-Game [CTRL]")
struct CTRLSTATETREE_API FCTRLComponentVisibilityInGameTask : public FCTRLComponentVisibilityTask
{
	GENERATED_BODY()

#if WITH_EDITOR
	virtual FText GetDescription(
		FGuid const& ID,
		FStateTreeDataView InstanceDataView,
		IStateTreeBindingLookup const& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text
	) const override;
#endif

protected:
	[[maybe_unused]] virtual void SetTargetVisibility(FStateTreeExecutionContext& Context, bool bNewVisibility) const override;
};
