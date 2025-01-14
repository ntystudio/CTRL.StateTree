// SPDX-FileCopyrightText: 2025 NTY.studio
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "Abilities/GameplayAbilityTypes.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"

#include "EstSetComponentVisibility.generated.h"

USTRUCT(BlueprintType)
struct FEstComponentVisibilityData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bTargetVisibility = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bPropagateToChildren = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
};

USTRUCT(BlueprintType, DisplayName="Set Component Visibility [EST]")
struct EXTENDEDSTATETREE_API FEstComponentVisibility : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

public:
	using FInstanceDataType = FEstComponentVisibilityData;
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

USTRUCT(DisplayName="Set Component Visibility In-Game [EST]")
struct EXTENDEDSTATETREE_API FEstComponentVisibilityInGame : public FEstComponentVisibility
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
