// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "CTRLAssignWidgetComponentTask.generated.h"

class UUserWidget;
class UWidgetComponent;

USTRUCT(meta=(Hidden, Category="Internal"))
struct CTRLSTATETREE_API FCTRLAssignWidgetComponentTaskData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UUserWidget> Widget;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UWidgetComponent> WidgetComponent;
};

/**
 * 
 */
USTRUCT(BlueprintType, DisplayName="Assign Widget to Component [CTRL]", meta=(Category="UI"))
struct CTRLSTATETREE_API FCTRLAssignWidgetComponentTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()
	using FInstanceDataType = FCTRLAssignWidgetComponentTaskData;

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
};
