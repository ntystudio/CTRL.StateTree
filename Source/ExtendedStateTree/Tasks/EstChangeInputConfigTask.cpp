#include "EstChangeInputConfigTask.h"

#include "StateTreeExecutionContext.h"
#include "TimerManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ExtendedStateTree/EstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstChangeInputConfigTask)

FEstInputModeConfig FEstChangeInputConfigTask::GetInputConfig(FInstanceDataType const* InstanceData) const
{
	if (InstanceData->bUseInputConfigPreset)
	{
		if (IsValid(InstanceData->InputConfigPreset))
		{
			return InstanceData->InputConfigPreset->InputConfig;
		} else {
			EST_LOG(Error, TEXT("FEstChangeInputConfigTask: InputConfigPreset is invalid %s"), *GetNameSafe(InstanceData->InputConfigPreset));
			return FEstInputModeConfig();
		}
	}
	 
	 return InstanceData->InputConfig;
}

EStateTreeRunStatus FEstChangeInputConfigTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<FEstChangeInputConfigTaskData>(*this);
	if (!IsValid(InstanceData.PlayerController))
	{
		EST_LOG(Error, TEXT("FEstChangeInputConfigTask: PlayerController is invalid %s"), *GetNameSafe(InstanceData.PlayerController));
		return EStateTreeRunStatus::Failed;
	}
	auto const ChangeInputConfigSubsystem = UEstChangeInputConfigSubsystem::Get(InstanceData.PlayerController);
	if (!ensure(ChangeInputConfigSubsystem)) { return EStateTreeRunStatus::Failed; }
	ensure(!InstanceData.InputConfigHandle.IsSet());
	auto const InputConfig = GetInputConfig(Context.GetInstanceDataPtr<FEstChangeInputConfigTaskData>(*this));
	InstanceData.InputConfigHandle = ChangeInputConfigSubsystem->PushInputConfig(InputConfig);
	return EStateTreeRunStatus::Running;
}

void FEstChangeInputConfigTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<FEstChangeInputConfigTaskData>(*this);
	if (!InstanceData.InputConfigHandle.IsSet()) { return; }

	ON_SCOPE_EXIT
	{
		InstanceData.InputConfigHandle.Reset();
	};

	auto const ChangeInputConfigSubsystem = UEstChangeInputConfigSubsystem::Get(InstanceData.PlayerController);
	if (!ChangeInputConfigSubsystem) return;
	auto const Handle = InstanceData.InputConfigHandle.GetValue();
	ChangeInputConfigSubsystem->PopInputConfig(Handle);
}

EDataValidationResult FEstChangeInputConfigTask::Compile(FStateTreeDataView const InstanceDataView, TArray<FText>& ValidationMessages)
{
	auto SuperResult = Super::Compile(InstanceDataView, ValidationMessages);
	auto const InstanceData = InstanceDataView.GetPtr<FEstChangeInputConfigTaskData>();
	if (InstanceData->bUseInputConfigPreset && !IsValid(InstanceData->InputConfigPreset))
	{
		SuperResult = EDataValidationResult::Invalid;
		ValidationMessages.Add(FText::FromString(FString::Printf(TEXT("InputConfigPreset is invalid: %s"), *GetNameSafe(InstanceData->InputConfigPreset))));
	}
	return SuperResult;
}
#if WITH_EDITOR
FText FEstChangeInputConfigTask::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	FInstanceDataType const* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	
	auto const InputConfig = GetInputConfig(InstanceData);
	auto Result = FString::Printf(
		TEXT("%s <s>Change Input</s> %s <b>%s</b>"),
		*UEstUtils::SymbolTaskContinuous,
		*UEstUtils::SymbolStateEnter,
		*UEnum::GetDisplayValueAsText(InputConfig.InputMode).ToString()
	);
	if (InstanceData->bUseInputConfigPreset) 
	{
		Result = Result.Append(FString::Printf(TEXT(" <b>(Preset: %s)</b>"), *GetNameSafe(InstanceData->InputConfigPreset)));
	}
	return UEstUtils::FormatDescription(Result, Formatting);
}

#endif
