// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"

#include "CTRLPrintTextTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLPrintTextTaskOnEventConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	bool bPrintToScreen = true;

	UPROPERTY(EditAnywhere)
	bool bPrintToLog = true;

	// include the current state name in the message
	UPROPERTY(EditAnywhere)
	bool bPrintStateName = true;

	UPROPERTY(EditAnywhere)
	FText Prefix = FText::GetEmpty();

	UPROPERTY(EditAnywhere)
	FText Message = FText::GetEmpty();

	UPROPERTY(EditAnywhere, meta=(EditCondition="bPrintToScreen"))
	FLinearColor TextColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bPrintToScreen", ClampMin="0.0", UIMin="0.0", UIMax="60.0", Units="s", Delta="1"))
	float Duration = 2.0f;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bPrintToScreen"))
	FName Key = NAME_None;
};

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLPrintTextTaskData
{
	GENERATED_BODY()

	FCTRLPrintTextTaskData();

	UPROPERTY(EditAnywhere)
	FCTRLPrintTextTaskOnEventConfig OnEnter;

	UPROPERTY(EditAnywhere)
	FCTRLPrintTextTaskOnEventConfig OnExit;
};

USTRUCT(BlueprintType, DisplayName="Print Text [CTRL]", meta=(Category="Debug"))
struct CTRLSTATETREE_API FCTRLPrintTextTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FCTRLPrintTextTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;

protected:
	virtual void Print(FStateTreeExecutionContext& Context, FCTRLPrintTextTaskOnEventConfig const& Config) const;

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
		return FName("CoreStyle|Log.TabIcon");
	}

#endif
};
