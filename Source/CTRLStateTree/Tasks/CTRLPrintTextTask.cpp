// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLStateTree/Tasks/CTRLPrintTextTask.h"

#include "PropertyHandle.h"
#include "StateTreeExecutionContext.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include "Engine/World.h"

#include "Kismet/KismetSystemLibrary.h"

FCTRLPrintTextTaskData::FCTRLPrintTextTaskData()
{
	OnEnter.Prefix = FText::FromString(UCTRLStateTreeUtils::SymbolStateEnter);
	OnExit.bEnabled = false;
	OnExit.Prefix = FText::FromString(UCTRLStateTreeUtils::SymbolStateExit);
	OnExit.TextColor = FLinearColor::Yellow;
}

EStateTreeRunStatus FCTRLPrintTextTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [OnEnter, OnExit] = Context.GetInstanceData<FInstanceDataType>(*this);
	Print(Context, OnEnter);
	return EStateTreeRunStatus::Running;
}

void FCTRLPrintTextTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [OnEnter, OnExit] = Context.GetInstanceData<FInstanceDataType>(*this);
	Print(Context, OnExit);
}

void FCTRLPrintTextTask::Print(FStateTreeExecutionContext& Context, FCTRLPrintTextTaskOnEventConfig const& Config) const
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

FText FCTRLPrintTextTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Result = FString::Printf(TEXT("%s<s>Print Text</s>"), *UCTRLStateTreeUtils::SymbolTaskContinuous);
	auto const& [OnEnter, OnExit] = InstanceDataView.Get<FInstanceDataType>();
	if (OnEnter.bEnabled)
	{
		Result = Result.Append(TEXT(" "));
		FStateTreePropertyPath const Path = UCTRLStateTreeUtils::GetStructPropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OnEnter), GET_MEMBER_NAME_CHECKED(FCTRLPrintTextTaskOnEventConfig, Message));
		auto const Msg = BindingLookup.GetBindingSourceDisplayName(Path, Formatting).ToString();
		Result = Result.Append(FString::Printf(TEXT("%s %s %s"), *UCTRLStateTreeUtils::SymbolStateEnter, *OnEnter.Prefix.ToString(), *Msg).TrimStartAndEnd());
	}
	if (OnExit.bEnabled)
	{
		Result = Result.Append(TEXT(" "));
		FStateTreePropertyPath const Path = UCTRLStateTreeUtils::GetStructPropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OnExit), GET_MEMBER_NAME_CHECKED(FCTRLPrintTextTaskOnEventConfig, Message));
		auto const Msg = BindingLookup.GetBindingSourceDisplayName(Path, Formatting).ToString();
		Result = Result.Append(FString::Printf(TEXT("%s %s %s"), *UCTRLStateTreeUtils::SymbolStateExit, *OnExit.Prefix.ToString(), *Msg).TrimStartAndEnd());
	}

	return UCTRLStateTreeUtils::FormatDescription(
		Result,
		Formatting
	);
}
#endif
