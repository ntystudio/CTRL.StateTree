// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UseStateTree/Tasks/UstPrintTextTask.h"

#include "PropertyHandle.h"
#include "StateTreeExecutionContext.h"

#include "Engine/World.h"

#include "Kismet/KismetSystemLibrary.h"

#include "UseStateTree/UstUtils.h"

FUstPrintTextTaskData::FUstPrintTextTaskData()
{
	OnEnter.Prefix = FText::FromString(UUstUtils::SymbolStateEnter);
	OnExit.bEnabled = false;
	OnExit.Prefix = FText::FromString(UUstUtils::SymbolStateExit);
	OnExit.TextColor = FLinearColor::Yellow;
}

EStateTreeRunStatus FUstPrintTextTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [OnEnter, OnExit] = Context.GetInstanceData<FInstanceDataType>(*this);
	Print(Context, OnEnter);
	return EStateTreeRunStatus::Running;
}

void FUstPrintTextTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [OnEnter, OnExit] = Context.GetInstanceData<FInstanceDataType>(*this);
	Print(Context, OnExit);
}

void FUstPrintTextTask::Print(FStateTreeExecutionContext& Context, FUstPrintTextConfig const& Config) const
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

FText FUstPrintTextTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Result = FString::Printf(TEXT("%s<s>Print Text</s>"), *UUstUtils::SymbolTaskContinuous);
	auto const& [OnEnter, OnExit] = InstanceDataView.Get<FInstanceDataType>();
	if (OnEnter.bEnabled)
	{
		Result = Result.Append(TEXT(" "));
		FStateTreePropertyPath const Path = UUstUtils::GetStructPropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OnEnter), GET_MEMBER_NAME_CHECKED(FUstPrintTextConfig, Message));
		auto const Msg = BindingLookup.GetBindingSourceDisplayName(Path, Formatting).ToString();
		Result = Result.Append(FString::Printf(TEXT("%s %s %s"), *UUstUtils::SymbolStateEnter, *OnEnter.Prefix.ToString(), *Msg).TrimStartAndEnd());
	}
	if (OnExit.bEnabled)
	{
		Result = Result.Append(TEXT(" "));
		FStateTreePropertyPath const Path = UUstUtils::GetStructPropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, OnExit), GET_MEMBER_NAME_CHECKED(FUstPrintTextConfig, Message));
		auto const Msg = BindingLookup.GetBindingSourceDisplayName(Path, Formatting).ToString();
		Result = Result.Append(FString::Printf(TEXT("%s %s %s"), *UUstUtils::SymbolStateExit, *OnExit.Prefix.ToString(), *Msg).TrimStartAndEnd());
	}

	return UUstUtils::FormatDescription(
		Result,
		Formatting
	);
}
#endif
