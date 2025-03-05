#include "CTRLChangeInputConfigTask.h"

#include "StateTreeExecutionContext.h"
#include "TimerManager.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLChangeInputConfigTask)

FCTRLInputModeConfig FCTRLChangeInputConfigTask::GetInputConfig(FInstanceDataType const* InstanceData) const
{
	if (InstanceData->bUseInputConfigPreset)
	{
		if (IsValid(InstanceData->InputConfigPreset))
		{
			return InstanceData->InputConfigPreset->InputConfig;
		} else {
			CTRLST_LOG(Error, TEXT("FCTRLChangeInputConfigTask: InputConfigPreset is invalid %s"), *GetNameSafe(InstanceData->InputConfigPreset));
			return FCTRLInputModeConfig();
		}
	}
	 
	 return InstanceData->InputConfig;
}

EStateTreeRunStatus FCTRLChangeInputConfigTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<FCTRLChangeInputConfigTaskData>(*this);
	if (!IsValid(InstanceData.PlayerController))
	{
		CTRLST_LOG(Error, TEXT("FCTRLChangeInputConfigTask: PlayerController is invalid %s"), *GetNameSafe(InstanceData.PlayerController));
		return EStateTreeRunStatus::Failed;
	}
	auto const ChangeInputConfigSubsystem = UCTRLChangeInputConfigSubsystem::Get(InstanceData.PlayerController);
	if (!ensure(ChangeInputConfigSubsystem)) { return EStateTreeRunStatus::Failed; }
	ensure(!InstanceData.InputConfigHandle.IsSet());
	auto const InputConfig = GetInputConfig(Context.GetInstanceDataPtr<FCTRLChangeInputConfigTaskData>(*this));
	InstanceData.InputConfigHandle = ChangeInputConfigSubsystem->PushInputConfig(InputConfig);
	return EStateTreeRunStatus::Running;
}

void FCTRLChangeInputConfigTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<FCTRLChangeInputConfigTaskData>(*this);
	if (!InstanceData.InputConfigHandle.IsSet()) { return; }

	ON_SCOPE_EXIT
	{
		InstanceData.InputConfigHandle.Reset();
	};

	auto const ChangeInputConfigSubsystem = UCTRLChangeInputConfigSubsystem::Get(InstanceData.PlayerController);
	if (!ChangeInputConfigSubsystem) return;
	auto const Handle = InstanceData.InputConfigHandle.GetValue();
	ChangeInputConfigSubsystem->PopInputConfig(Handle);
}

EDataValidationResult FCTRLChangeInputConfigTask::Compile(FStateTreeDataView const InstanceDataView, TArray<FText>& ValidationMessages)
{
	auto SuperResult = Super::Compile(InstanceDataView, ValidationMessages);
	auto const InstanceData = InstanceDataView.GetPtr<FCTRLChangeInputConfigTaskData>();
	if (InstanceData->bUseInputConfigPreset && !IsValid(InstanceData->InputConfigPreset))
	{
		SuperResult = EDataValidationResult::Invalid;
		ValidationMessages.Add(FText::FromString(FString::Printf(TEXT("InputConfigPreset is invalid: %s"), *GetNameSafe(InstanceData->InputConfigPreset))));
	}
	return SuperResult;
}
#if WITH_EDITOR
FText FCTRLChangeInputConfigTask::GetDescription(
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
		*UCTRLStateTreeUtils::SymbolTaskContinuous,
		*UCTRLStateTreeUtils::SymbolStateEnter,
		*UEnum::GetDisplayValueAsText(InputConfig.InputMode).ToString()
	);
	if (InstanceData->bUseInputConfigPreset) 
	{
		Result = Result.Append(FString::Printf(TEXT(" <b>(Preset: %s)</b>"), *GetNameSafe(InstanceData->InputConfigPreset)));
	}
	return UCTRLStateTreeUtils::FormatDescription(Result, Formatting);
}

#endif
