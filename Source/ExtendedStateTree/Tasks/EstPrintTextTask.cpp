// SPDX-FileCopyrightText: 2025 NTY.studio

#include "ExtendedStateTree/Tasks/EstPrintTextTask.h"

#include "PropertyHandle.h"
#include "StateTreeExecutionContext.h"

#include "Engine/World.h"
#include "ExtendedStateTree/EstUtils.h"

#include "Kismet/KismetSystemLibrary.h"

FEstPrintTextTaskData::FEstPrintTextTaskData()
{
	OnEnter.Prefix = FText::FromString(UEstUtils::SymbolStateEnter);
	OnExit.bEnabled = false;
	OnExit.Prefix = FText::FromString(UEstUtils::SymbolStateExit);
	OnExit.TextColor = FLinearColor::Yellow;
}

EStateTreeRunStatus FEstPrintTextTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [OnEnter, OnExit] = Context.GetInstanceData<FInstanceDataType>(*this);
	Print(Context, OnEnter);
	return EStateTreeRunStatus::Running;
}

void FEstPrintTextTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [OnEnter, OnExit] = Context.GetInstanceData<FInstanceDataType>(*this);
	Print(Context, OnExit);
}

void FEstPrintTextTask::Print(FStateTreeExecutionContext& Context, FEstPrintTextConfig const& Config) const
{
	if (!Config.bEnabled) return;
	auto Prefix = Config.Prefix.ToString();
	if (Config.bPrintStateName)
	{
		Prefix = Prefix.Append("\n");
		Prefix = Prefix.Append(Context.GetActiveStateName());
	}
	Prefix = Prefix.Append(": ");
	UKismetSystemLibrary::PrintString(Context.GetWorld(), Prefix + Config.Message.ToString(), Config.bPrintToScreen, Config.bPrintToLog, Config.TextColor, Config.Duration);
}

#if WITH_EDITOR

FText FEstPrintTextTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Result = FString::Printf(TEXT("%s<s>Print Text</s>"), *UEstUtils::SymbolTaskContinuous);
	auto const& [OnEnter, OnExit] = InstanceDataView.Get<FInstanceDataType>();
	if (OnEnter.bEnabled)
	{
		Result = Result.Append(TEXT(" "));
		FStateTreePropertyPath const Path = UEstUtils::GetStructPropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OnEnter), GET_MEMBER_NAME_CHECKED(FEstPrintTextConfig, Message));
		auto const Msg = BindingLookup.GetBindingSourceDisplayName(Path, Formatting).ToString();
		Result = Result.Append(FString::Printf(TEXT("%s %s %s"), *UEstUtils::SymbolStateEnter, *OnEnter.Prefix.ToString(), *Msg).TrimStartAndEnd());
	}
	if (OnExit.bEnabled)
	{
		Result = Result.Append(TEXT(" "));
		FStateTreePropertyPath const Path = UEstUtils::GetStructPropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OnExit), GET_MEMBER_NAME_CHECKED(FEstPrintTextConfig, Message));
		auto const Msg = BindingLookup.GetBindingSourceDisplayName(Path, Formatting).ToString();
		Result = Result.Append(FString::Printf(TEXT("%s %s %s"), *UEstUtils::SymbolStateExit, *OnExit.Prefix.ToString(), *Msg).TrimStartAndEnd());
	}

	return UEstUtils::FormatDescription(
		Result,
		Formatting
	);
}
#endif
