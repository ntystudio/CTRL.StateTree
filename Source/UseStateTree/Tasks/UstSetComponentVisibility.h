// SPDX-FileCopyrightText: 2025 NTY.studio
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "UseStateTree/Tasks/UstStateTreeTaskCommonBase.h"

#include "UstSetComponentVisibility.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FUstComponentVisibilityData
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

USTRUCT(BlueprintType, DisplayName="Set Component Visibility [UST]", meta=(Category="Component"))
struct USESTATETREE_API FUstComponentVisibility : public FUstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FUstComponentVisibilityData;
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

USTRUCT(DisplayName="Set Component Visibility In-Game [UST]")
struct USESTATETREE_API FUstComponentVisibilityInGame : public FUstComponentVisibility
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
