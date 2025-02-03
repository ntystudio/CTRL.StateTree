// SPDX-FileCopyrightText: 2025 NTY.studio

#include "UstTriggerGameplayEventTask.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

#include "UseStateTree/UseStateTree.h"
#include "UseStateTree/UstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UstTriggerGameplayEventTask)

EStateTreeRunStatus FUstTriggerGameplayEventTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto const Data = Context.GetInstanceData<FUstTriggerGameplayEventTaskData>(*this);
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

void FUstTriggerGameplayEventTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto const Data = Context.GetInstanceData<FUstTriggerGameplayEventTaskData>(*this);
	if (Data.bUseExitGameplayEvent)
	{
		// ReSharper disable once CppExpressionWithoutSideEffects
		SendGameplayEvent(Data.ExitGameplayEvent, Data.TargetActors);
	}
}

bool FUstTriggerGameplayEventTask::SendGameplayEvent(FGameplayEventData const& EventData, TArray<AActor*> const& TargetActors) const
{
	if (EventData.Target)
	{
		UST_CLOG(bDebugEnabled, Log, TEXT("Sending Gameplay Event %s to Event Target %s"), *EventData.EventTag.ToString(), *GetNameSafe(EventData.Target));
		AActor const* Target = EventData.Target;

		if (!IsValid(Target))
		{
			UST_LOG(Warning, TEXT("Invalid Target Actor"));
			return false;
		}
		if (!IsValid(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target)))
		{
			UST_LOG(Warning, TEXT("Target Actor has no ASC %s"), *GetNameSafe(Target));
			return false;
		}
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(const_cast<AActor*>(Target), EventData.EventTag, EventData);
		return true;
	}
	auto ValidActors = TargetActors.FilterByPredicate([](AActor const* Actor) { return IsValid(Actor) && IsValid(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor)); });

	UST_CLOG(bDebugEnabled, Log, TEXT("Sending Gameplay Event %s to %d Actors"), *EventData.EventTag.ToString(), ValidActors.Num());
	for (auto const Actor : ValidActors)
	{
		UST_CLOG(bDebugEnabled, Log, TEXT("\tSending Gameplay Event %s to %s"), *EventData.EventTag.ToString(), *Actor->GetName());
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventData.EventTag, EventData);
	}
	return ValidActors.Num() > 0;
}

#if WITH_EDITOR
FText FUstTriggerGameplayEventTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	FString Out = TEXT("<s>Trigger Gameplay Event</s> ");
	FInstanceDataType const* Data = InstanceDataView.GetPtr<FInstanceDataType>();
	if (Data->bUseEnterGameplayEvent)
	{
		FStateTreePropertyPath const EventTagPath = UUstUtils::GetStructPropertyPath(
			ID,
			GET_MEMBER_NAME_CHECKED(FInstanceDataType, EnterGameplayEvent),
			GET_MEMBER_NAME_CHECKED(FGameplayEventData, EventTag)
		);
		auto const BindingSource = BindingLookup.GetPropertyBindingSource(EventTagPath);
		auto const EventTagMsg = BindingSource ? BindingLookup.GetBindingSourceDisplayName(EventTagPath, Formatting).ToString() : Data->EnterGameplayEvent.EventTag.ToString();
		Out = Out.Append(FString::Printf(TEXT("%s %s "), *UUstUtils::SymbolStateEnter, *EventTagMsg));
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
		FStateTreePropertyPath const EventTagPath = UUstUtils::GetStructPropertyPath(
			ID,
			GET_MEMBER_NAME_CHECKED(FInstanceDataType, ExitGameplayEvent),
			GET_MEMBER_NAME_CHECKED(FGameplayEventData, EventTag)
		);
		auto const BindingSource = BindingLookup.GetPropertyBindingSource(EventTagPath);
		auto const EventTagMsg = BindingSource ? BindingLookup.GetBindingSourceDisplayName(EventTagPath, Formatting).ToString() : Data->ExitGameplayEvent.EventTag.ToString();
		Out = Out.Append(FString::Printf(TEXT("%s %s "), *UUstUtils::SymbolStateExit, *EventTagMsg));
		if (Data->ExitGameplayEvent.Target)
		{
			Out = Out.Append(FString::Printf(TEXT("<s>on</s> %s "), *Data->EnterGameplayEvent.Target->GetName()));
		}
		else
		{
			Out = Out.Append(FString::Printf(TEXT("<s>on</s> %d <s>Actors</s>"), Data->TargetActors.Num()));
		}
	}
	return UUstUtils::FormatDescription(Out, Formatting);
}
#endif
