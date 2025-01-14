// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "UObject/Object.h"

#include "EstPrintTextTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden))
struct FEstPrintTextConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled = true;

	UPROPERTY(EditAnywhere)
	bool bPrintToScreen = true;

	UPROPERTY(EditAnywhere)
	bool bPrintToLog = true;

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

USTRUCT(BlueprintType, meta=(Category="Debug", Hidden))
struct FEstPrintTextTaskData
{
	GENERATED_BODY()

	FEstPrintTextTaskData()
	{
		OnEnter.Prefix = FText::FromString("Enter");
		OnExit.bEnabled = false;
		OnExit.Prefix = FText::FromString("Exit");
		OnExit.TextColor = FLinearColor::Yellow;
	}

	UPROPERTY(EditAnywhere)
	FEstPrintTextConfig OnEnter;

	UPROPERTY(EditAnywhere)
	FEstPrintTextConfig OnExit;
};

USTRUCT(BlueprintType, DisplayName="Print Text [EST]", meta=(Category="Debug"))
struct EXTENDEDSTATETREE_API FEstPrintTextTask : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FEstPrintTextTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;

protected:
	virtual void Print(FStateTreeExecutionContext& Context, FEstPrintTextConfig const& Config) const;

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
