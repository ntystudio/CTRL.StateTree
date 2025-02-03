#include "UstChangeInputConfigTask.h"

#include "StateTreeExecutionContext.h"
#include "TimerManager.h"

#include "UseStateTree/UstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UstChangeInputConfigTask)

FUstInputModeConfig FUstChangeInputConfigTask::GetInputConfig(FInstanceDataType const* InstanceData) const
{
	if (InstanceData->bUseInputConfigPreset)
	{
		if (IsValid(InstanceData->InputConfigPreset))
		{
			return InstanceData->InputConfigPreset->InputConfig;
		} else {
			UST_LOG(Error, TEXT("FUstChangeInputConfigTask: InputConfigPreset is invalid %s"), *GetNameSafe(InstanceData->InputConfigPreset));
			return FUstInputModeConfig();
		}
	}
	 
	 return InstanceData->InputConfig;
}

EStateTreeRunStatus FUstChangeInputConfigTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<FUstChangeInputConfigTaskData>(*this);
	if (!IsValid(InstanceData.PlayerController))
	{
		UST_LOG(Error, TEXT("FUstChangeInputConfigTask: PlayerController is invalid %s"), *GetNameSafe(InstanceData.PlayerController));
		return EStateTreeRunStatus::Failed;
	}
	auto const ChangeInputConfigSubsystem = UUstChangeInputConfigSubsystem::Get(InstanceData.PlayerController);
	if (!ensure(ChangeInputConfigSubsystem)) { return EStateTreeRunStatus::Failed; }
	ensure(!InstanceData.InputConfigHandle.IsSet());
	auto const InputConfig = GetInputConfig(Context.GetInstanceDataPtr<FUstChangeInputConfigTaskData>(*this));
	InstanceData.InputConfigHandle = ChangeInputConfigSubsystem->PushInputConfig(InputConfig);
	return EStateTreeRunStatus::Running;
}

void FUstChangeInputConfigTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<FUstChangeInputConfigTaskData>(*this);
	if (!InstanceData.InputConfigHandle.IsSet()) { return; }

	ON_SCOPE_EXIT
	{
		InstanceData.InputConfigHandle.Reset();
	};

	auto const ChangeInputConfigSubsystem = UUstChangeInputConfigSubsystem::Get(InstanceData.PlayerController);
	if (!ChangeInputConfigSubsystem) return;
	auto const Handle = InstanceData.InputConfigHandle.GetValue();
	ChangeInputConfigSubsystem->PopInputConfig(Handle);
}

EDataValidationResult FUstChangeInputConfigTask::Compile(FStateTreeDataView const InstanceDataView, TArray<FText>& ValidationMessages)
{
	auto SuperResult = Super::Compile(InstanceDataView, ValidationMessages);
	auto const InstanceData = InstanceDataView.GetPtr<FUstChangeInputConfigTaskData>();
	if (InstanceData->bUseInputConfigPreset && !IsValid(InstanceData->InputConfigPreset))
	{
		SuperResult = EDataValidationResult::Invalid;
		ValidationMessages.Add(FText::FromString(FString::Printf(TEXT("InputConfigPreset is invalid: %s"), *GetNameSafe(InstanceData->InputConfigPreset))));
	}
	return SuperResult;
}
#if WITH_EDITOR
FText FUstChangeInputConfigTask::GetDescription(
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
		*UUstUtils::SymbolTaskContinuous,
		*UUstUtils::SymbolStateEnter,
		*UEnum::GetDisplayValueAsText(InputConfig.InputMode).ToString()
	);
	if (InstanceData->bUseInputConfigPreset) 
	{
		Result = Result.Append(FString::Printf(TEXT(" <b>(Preset: %s)</b>"), *GetNameSafe(InstanceData->InputConfigPreset)));
	}
	return UUstUtils::FormatDescription(Result, Formatting);
}

#endif
