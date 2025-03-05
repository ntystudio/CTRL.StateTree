// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLForwardGameplayEventToStateTreeTask.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

#include "CTRLStateTree/CTRLStateTree.h"
#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLForwardGameplayEventToStateTreeTask)

#define LOCTEXT_NAMESPACE "GameplayEventToStateTreeEventTask"

void UCTRLEventBridge::GameplayEventCallback(FGameplayEventData const* GameplayEventData) const
{
	if (!GameplayEventData) { return; }
	GameplayEventContainerCallback(GameplayEventData->EventTag, GameplayEventData);
}

void UCTRLEventBridge::GameplayEventContainerCallback(FGameplayTag const GameplayTag, FGameplayEventData const* GameplayEventData) const
{
	if (!GameplayEventData || !GameplayTag.IsValid()) { return; }
	if (EventReceived.IsBound())
	{
		FGameplayEventData TempPayload = *GameplayEventData;
		TempPayload.EventTag = GameplayTag;
		EventReceived.Execute(MoveTemp(TempPayload));
	}
}

EDataValidationResult FCTRLForwardGameplayEventToStateTreeTask::Compile(FStateTreeDataView const InstanceDataView, TArray<FText>& ValidationMessages)
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

UCTRLEventBridge* FCTRLForwardGameplayEventToStateTreeTask::MakeListener(FStateTreeExecutionContext const& Context) const
{
	auto& [Actor, EventTag, bOnlyMatchExact, bOnlyTriggerOnce, Listener, DelegateHandle] = Context.GetInstanceData<FInstanceDataType>(*this);
	if (Listener)
	{
		Listener->EventReceived.Unbind();
		Listener->RemoveFromRoot();
		Listener = nullptr;
	}
	DelegateHandle.Reset();

	Listener = NewObject<UCTRLEventBridge>();
	Listener->AddToRoot();
	FStateTreeEventQueue& EventQueue = Context.GetMutableEventQueue();
	FString MsgPart = FString::Printf(TEXT("%s. Sending → StateTree %s"), *GetNameSafe(Actor), *GetNameSafe(Context.GetStateTree()));
	Listener->EventReceived.BindWeakLambda(
		Actor.Get(),
		[bDebugEnabled = bDebugEnabled, InstanceDataRef = Context.GetInstanceDataStructRef(*this), &EventQueue, Owner = Context.GetOwner(), MsgPart](FGameplayEventData Payload)
		{
			if (FInstanceDataType* InstanceData = InstanceDataRef.GetPtr())
			{
				CTRLST_CLOG(bDebugEnabled, Warning, TEXT("Received gameplay event %s %s"), *Payload.EventTag.ToString(), *MsgPart);
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

EStateTreeRunStatus FCTRLForwardGameplayEventToStateTreeTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& [Actor, EventTag, bOnlyMatchExact, bOnlyTriggerOnce, Listener, DelegateHandle] = Context.GetInstanceData<FInstanceDataType>(*this);
	Listener = MakeListener(Context);
	if (auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		if (bOnlyMatchExact)
		{
			DelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EventTag).AddUObject(Listener, &UCTRLEventBridge::GameplayEventCallback);
		}
		else
		{
			DelegateHandle = ASC->AddGameplayEventTagContainerDelegate(
				FGameplayTagContainer(EventTag),
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(Listener, &UCTRLEventBridge::GameplayEventContainerCallback)
			);
		}
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

void FCTRLForwardGameplayEventToStateTreeTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
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
#if WITH_EDITOR
FText FCTRLForwardGameplayEventToStateTreeTask::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const ActorName = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Actor)), Formatting).ToString();
	auto const TagName = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EventTag)), Formatting).ToString();
	return UCTRLStateTreeUtils::FormatDescription(
		FString::Printf(TEXT("%s<s>Gameplay → StateTree Event:</s> %s <s>on</s> %s"), *UCTRLStateTreeUtils::SymbolTaskContinuous, *TagName, *ActorName),
		Formatting
	);
}
#endif

#undef LOCTEXT_NAMESPACE
