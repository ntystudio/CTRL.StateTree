// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLGasTriggerEventTask.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

#include "CTRLStateTree/CTRLStateTree.h"
#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLGasTriggerEventTask)

EStateTreeRunStatus FCTRLGasTriggerEventTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto const Data = Context.GetInstanceData<FCTRLGasTriggerEventTaskData>(*this);
	if (Data.bUseEnterGameplayEvent)
	{
		bool const bSent = SendGameplayEvent(Data.EnterGameplayEvent, Data.TargetActors);
		if (!bSent && Data.bFailIfNotSent)
		{
			return EStateTreeRunStatus::Failed;
		}
	}
	return EStateTreeRunStatus::Running;
}

void FCTRLGasTriggerEventTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto const Data = Context.GetInstanceData<FCTRLGasTriggerEventTaskData>(*this);
	if (Data.bUseExitGameplayEvent)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		SendGameplayEvent(Data.ExitGameplayEvent, Data.TargetActors);
	}
}

bool FCTRLGasTriggerEventTask::SendGameplayEvent(FGameplayEventData const& EventData, TArray<AActor*> const& TargetActors) const
{
	if (EventData.Target)
	{
		CTRLST_CLOG(bDebugEnabled, Log, TEXT("Sending Gameplay Event %s to Event Target %s"), *EventData.EventTag.ToString(), *GetNameSafe(EventData.Target));
		AActor const* Target = EventData.Target;

		if (!IsValid(Target))
		{
			CTRLST_LOG(Warning, TEXT("Invalid Target Actor"));
			return false;
		}
		if (!IsValid(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target)))
		{
			CTRLST_LOG(Warning, TEXT("Target Actor has no ASC %s"), *GetNameSafe(Target));
			return false;
		}
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(Target), EventData.EventTag, EventData);
		return true;
	}
	auto ValidActors = TargetActors.FilterByPredicate([](AActor const* Actor) { return IsValid(Actor) && IsValid(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)); });

	CTRLST_CLOG(bDebugEnabled, Log, TEXT("Sending Gameplay Event %s to %d Actors"), *EventData.EventTag.ToString(), ValidActors.Num());
	for (auto const Actor : ValidActors)
	{
		CTRLST_CLOG(bDebugEnabled, Log, TEXT("\tSending Gameplay Event %s to %s"), *EventData.EventTag.ToString(), *Actor->GetName());
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventData.EventTag, EventData);
	}
	return ValidActors.Num() > 0;
}

#if WITH_EDITOR
FText FCTRLGasTriggerEventTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	FString Out = TEXT("<s>Trigger Gameplay Event</s> ");
	FInstanceDataType const* Data = InstanceDataView.GetPtr<FInstanceDataType>();
	if (Data->bUseEnterGameplayEvent)
	{
		FStateTreePropertyPath const EventTagPath = UCTRLStateTreeUtils::GetStructPropertyPath(
			ID,
			GET_MEMBER_NAME_CHECKED(FInstanceDataType, EnterGameplayEvent),
			GET_MEMBER_NAME_CHECKED(FGameplayEventData, EventTag)
		);
		auto const BindingSource = BindingLookup.GetPropertyBindingSource(EventTagPath);
		auto const EventTagMsg = BindingSource ? BindingLookup.GetBindingSourceDisplayName(EventTagPath, Formatting).ToString() : Data->EnterGameplayEvent.EventTag.ToString();
		Out = Out.Append(FString::Printf(TEXT("%s %s "), *UCTRLStateTreeUtils::SymbolStateEnter, *EventTagMsg));
		if (Data->EnterGameplayEvent.Target)
		{
			Out = Out.Append(FString::Printf(TEXT("<s>on</s> %s "), *Data->EnterGameplayEvent.Target->GetName()));
		}
		else
		{
			Out = Out.Append(FString::Printf(TEXT("<s>on</s> %d <s>Actors</s>"), Data->TargetActors.Num()));
		}
	}
	if (Data->bUseExitGameplayEvent)
	{
		FStateTreePropertyPath const EventTagPath = UCTRLStateTreeUtils::GetStructPropertyPath(
			ID,
			GET_MEMBER_NAME_CHECKED(FInstanceDataType, ExitGameplayEvent),
			GET_MEMBER_NAME_CHECKED(FGameplayEventData, EventTag)
		);
		auto const BindingSource = BindingLookup.GetPropertyBindingSource(EventTagPath);
		auto const EventTagMsg = BindingSource ? BindingLookup.GetBindingSourceDisplayName(EventTagPath, Formatting).ToString() : Data->ExitGameplayEvent.EventTag.ToString();
		Out = Out.Append(FString::Printf(TEXT("%s %s "), *UCTRLStateTreeUtils::SymbolStateExit, *EventTagMsg));
		if (Data->ExitGameplayEvent.Target)
		{
			Out = Out.Append(FString::Printf(TEXT("<s>on</s> %s "), *Data->EnterGameplayEvent.Target->GetName()));
		}
		else
		{
			Out = Out.Append(FString::Printf(TEXT("<s>on</s> %d <s>Actors</s>"), Data->TargetActors.Num()));
		}
	}
	return UCTRLStateTreeUtils::FormatDescription(Out, Formatting);
}
#endif
