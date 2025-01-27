#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"

#include "ExtendedStateTree/Tasks/EstStateTreeTaskCommonBase.h"
#include "ExtendedStateTree/Utils/EstChangeInputConfigSubsystem.h"

#include "EstChangeInputConfigTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FEstChangeInputConfigTaskData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(InlineEditConditionToggle))
	bool bUseInputConfigPreset = false;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(EditCondition="bUseInputConfigPreset"))
	TObjectPtr<UEstInputConfigPreset> InputConfigPreset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(EditCondition="!bUseInputConfigPreset"))
	FEstInputModeConfig InputConfig;

	UPROPERTY(Transient)
	TOptional<FGuid> InputConfigHandle;
};

USTRUCT(BlueprintType, DisplayName = "Change Input [EST]", meta = (Category = "Input"))
struct FEstChangeInputConfigTask : public FEstStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstChangeInputConfigTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	FEstInputModeConfig GetInputConfig(FInstanceDataType const* InstanceData) const;
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
#endif
};
