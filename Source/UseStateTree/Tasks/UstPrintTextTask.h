// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"

#include "UseStateTree/Tasks/UstStateTreeTaskCommonBase.h"

#include "UstPrintTextTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FUstPrintTextConfig
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
struct FUstPrintTextTaskData
{
	GENERATED_BODY()

	FUstPrintTextTaskData();

	UPROPERTY(EditAnywhere)
	FUstPrintTextConfig OnEnter;

	UPROPERTY(EditAnywhere)
	FUstPrintTextConfig OnExit;
};

USTRUCT(BlueprintType, DisplayName="Print Text [UST]", meta=(Category="Debug"))
struct USESTATETREE_API FUstPrintTextTask : public FUstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FUstPrintTextTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const override;

protected:
	virtual void Print(FStateTreeExecutionContext& Context, FUstPrintTextConfig const& Config) const;

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
