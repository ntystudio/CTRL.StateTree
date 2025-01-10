// SPDX-FileCopyrightText: 2025 NTY.studio

#include "EstForwardGameplayEventToStateTreeTask.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

#include "ExtendedStateTree/ExtendedStateTree.h"
#include "ExtendedStateTree/EstUtils.h"

#define LOCTEXT_NAMESPACE "GameplayEventToStateTreeEventTask"

void UEstEventBridge::GameplayEventCallback(FGameplayEventData const* GameplayEventData) const
{
	if (!GameplayEventData) { return; }
	GameplayEventContainerCallback(GameplayEventData->EventTag, GameplayEventData);
}

void UEstEventBridge::GameplayEventContainerCallback(FGameplayTag const GameplayTag, FGameplayEventData const* GameplayEventData) const
{
	if (!GameplayEventData || !GameplayTag.IsValid()) { return; }
	if (EventReceived.IsBound())
	{
		FGameplayEventData TempPayload = *GameplayEventData;
		TempPayload.EventTag = GameplayTag;
		EventReceived.Execute(MoveTemp(TempPayload));
	}
}

EDataValidationResult FEstForwardGameplayEventToStateTreeTask::Compile(FStateTreeDataView const InstanceDataView, TArray<FText>& ValidationMessages)
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	auto const* Data = InstanceDataView.GetPtr<FInstanceDataType>();
	if (!Data->EventTag.IsValid())
	{
		ValidationMessages.Add(LOCTEXT("MissingTag", "Tag property is empty, expecting valid tag."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

UEstEventBridge* FEstForwardGameplayEventToStateTreeTask::MakeListener(FStateTreeExecutionContext const& Context) const
{
	auto& [Actor, EventTag, bOnlyMatchExact, bOnlyTriggerOnce, Listener, DelegateHandle] = Context.GetInstanceData<FInstanceDataType>(*this);
	if (Listener)
	{
		Listener->EventReceived.Unbind();
		Listener->RemoveFromRoot();
		Listener = nullptr;
	}
	DelegateHandle.Reset();

	Listener = NewObject<UEstEventBridge>();
	Listener->AddToRoot();
	FStateTreeEventQueue& EventQueue = Context.GetMutableEventQueue();
	FString MsgPart = FString::Printf(TEXT("%s. Sending to StateTree %s"), *GetNameSafe(Actor), *GetNameSafe(Context.GetStateTree()));
	Listener->EventReceived.BindWeakLambda(
		Actor.Get(),
		[bDebugEnabled = bDebugEnabled, InstanceDataRef = Context.GetInstanceDataStructRef(*this), &EventQueue, Owner = Context.GetOwner(), MsgPart](FGameplayEventData Payload)
		{
			if (FInstanceDataType* InstanceData = InstanceDataRef.GetPtr())
			{
				EST_CLOG(bDebugEnabled, Warning, TEXT("Received gameplay event %s %s"), *Payload.EventTag.ToString(), *MsgPart);
				if (InstanceData->bOnlyTriggerOnce)
				{
					InstanceData->Bridge->EventReceived.Unbind();
					InstanceData->Bridge->RemoveFromRoot();
					InstanceData->Bridge = nullptr;
				}
				FStructView const StructView = FStructView::Make(Payload.TargetData);
				EventQueue.SendEvent(Owner, Payload.EventTag, StructView);
			}
		}
	);
	return Listener;
}

EStateTreeRunStatus FEstForwardGameplayEventToStateTreeTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [Actor, EventTag, bOnlyMatchExact, bOnlyTriggerOnce, Listener, DelegateHandle] = Context.GetInstanceData<FInstanceDataType>(*this);
	Listener = MakeListener(Context);
	if (auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		if (bOnlyMatchExact)
		{
			DelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EventTag).AddUObject(Listener, &UEstEventBridge::GameplayEventCallback);
		}
		else
		{
			DelegateHandle = ASC->AddGameplayEventTagContainerDelegate(
				FGameplayTagContainer(EventTag),
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(Listener, &UEstEventBridge::GameplayEventContainerCallback)
			);
		}
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

void FEstForwardGameplayEventToStateTreeTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [Actor, EventTag, bOnlyMatchExact, bOnlyTriggerOnce, Listener, DelegateHandle] = Context.GetInstanceData<FInstanceDataType>(*this);
	auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	if (ASC && DelegateHandle.IsValid())
	{
		if (bOnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(EventTag).Remove(DelegateHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(EventTag), DelegateHandle);
		}
	}

	if (Listener)
	{
		Listener->EventReceived.Unbind();
		Listener->RemoveFromRoot();
		Listener = nullptr;
	}
}

FText FEstForwardGameplayEventToStateTreeTask::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const* Data = InstanceDataView.GetPtr<FInstanceDataType>();
	return UEstUtils::FormatDescription(
		FString::Printf(TEXT("%s<s>Gameplay -> StateTree Event:</s> %s"), *UEstUtils::SymbolTaskContinuous, *Data->EventTag.ToString()),
		Formatting
	);
}


#undef LOCTEXT_NAMESPACE
