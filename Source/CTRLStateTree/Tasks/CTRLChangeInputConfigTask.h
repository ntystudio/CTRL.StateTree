#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"

#include "CTRLStateTree/Tasks/CTRLStateTreeCommonBaseTask.h"
#include "CTRLStateTree/Utils/CTRLChangeInputConfigSubsystem.h"

#include "CTRLChangeInputConfigTask.generated.h"

USTRUCT(BlueprintType, meta=(Hidden, Category="Internal"))
struct FCTRLChangeInputConfigTaskData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(InlineEditConditionToggle))
	bool bUseInputConfigPreset = false;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(EditCondition="bUseInputConfigPreset"))
	TObjectPtr<UCTRLInputConfigPreset> InputConfigPreset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter", meta=(EditCondition="!bUseInputConfigPreset"))
	FCTRLInputModeConfig InputConfig;

	UPROPERTY(Transient)
	TOptional<FGuid> InputConfigHandle;
};

USTRUCT(BlueprintType, DisplayName = "Change Input [CTRL]", meta = (Category = "Input"))
struct FCTRLChangeInputConfigTask : public FCTRLStateTreeCommonBaseTask
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLChangeInputConfigTaskData;
	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	FCTRLInputModeConfig GetInputConfig(FInstanceDataType const* InstanceData) const;
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
