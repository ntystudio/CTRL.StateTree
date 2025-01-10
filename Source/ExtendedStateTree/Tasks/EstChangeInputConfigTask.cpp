#include "EstChangeInputConfigTask.h"

#include "StateTreeExecutionContext.h"
#include "TimerManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "ExtendedStateTree/EstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstChangeInputConfigTask)

EStateTreeRunStatus FEstChangeInputConfigTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [PlayerController, InputConfig, InputConfigHandle] = Context.GetInstanceData<FEstChangeInputConfigTaskData>(*this);
	if (!IsValid(PlayerController))
	{
		EST_LOG(Error, TEXT("FEstChangeInputConfigTask: PlayerController is invalid %s"), *GetNameSafe(PlayerController));
		return EStateTreeRunStatus::Failed;
	}
	auto const ChangeInputConfigSubsystem = UEstChangeInputConfigSubsystem::Get(PlayerController);
	if (!ensure(ChangeInputConfigSubsystem)) { return EStateTreeRunStatus::Failed; }
	ensure(!InputConfigHandle.IsSet());
	InputConfigHandle = ChangeInputConfigSubsystem->PushInputConfig(InputConfig);
	return EStateTreeRunStatus::Running;
}

void FEstChangeInputConfigTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [PlayerController, InputConfig, InputConfigHandle] = Context.GetInstanceData<FEstChangeInputConfigTaskData>(*this);
	if (!InputConfigHandle.IsSet()) { return; }

	ON_SCOPE_EXIT
	{
		InputConfigHandle.Reset();
	};

	auto const ChangeInputConfigSubsystem = UEstChangeInputConfigSubsystem::Get(PlayerController);
	if (!ChangeInputConfigSubsystem) return;
	auto const Handle = InputConfigHandle.GetValue();
	ChangeInputConfigSubsystem->PopInputConfig(Handle);
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
	auto const Result = FString::Printf(
		TEXT("%s <s>Change Input</s> %s <b>%s</b>"),
		*UEstUtils::SymbolTaskContinuous,
		*UEstUtils::SymbolStateEnter,
		*UEnum::GetDisplayValueAsText(InstanceData->InputConfig.InputMode).ToString()
	);
	return UEstUtils::FormatDescription(Result, Formatting);
}
#endif
