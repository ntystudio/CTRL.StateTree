// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLGasEventToStateTreeEventTask.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"

#include "CTRLStateTree/CTRLStateTree.h"
#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLGasEventToStateTreeEventTask)

#define LOCTEXT_NAMESPACE "GameplayEventToStateTreeEventTask"

void UCTRLStateTreeEventBridge::GameplayEventCallback(FGameplayEventData const* GameplayEventData) const
{
	if (!GameplayEventData) { return; }
	GameplayEventContainerCallback(GameplayEventData->EventTag, GameplayEventData);
}

void UCTRLStateTreeEventBridge::GameplayEventContainerCallback(FGameplayTag const GameplayTag, FGameplayEventData const* GameplayEventData) const
{
	if (!GameplayEventData || !GameplayTag.IsValid()) { return; }
	if (EventReceived.IsBound())
	{
		FGameplayEventData TempPayload = *GameplayEventData;
		TempPayload.EventTag = GameplayTag;
		EventReceived.Execute(MoveTemp(TempPayload));
	}
}

EDataValidationResult FCTRLGasEventToStateTreeEventTask::Compile(FStateTreeDataView const InstanceDataView, TArray<FText>& ValidationMessages)
{
	auto const SuperResult = FCTRLStateTreeCommonBaseTask::Compile(InstanceDataView, ValidationMessages);
	EDataValidationResult Result = EDataValidationResult::Valid;

	auto const* Data = InstanceDataView.GetPtr<FInstanceDataType>();
	if (!Data->EventTags.IsValid())
	{
		ValidationMessages.Add(LOCTEXT("MissingTag", "EventTags is set to required, requires valid tag."));
		Result = EDataValidationResult::Invalid;
	}

	return CombineDataValidationResults(SuperResult, Result);
}

UCTRLStateTreeEventBridge* FCTRLGasEventToStateTreeEventTask::MakeBridge(FStateTreeExecutionContext const& Context) const
{
	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	auto Bridge = Data.Bridge;
	auto const Actor = Data.Actor;
	if (Bridge)
	{
		Bridge->EventReceived.Unbind();
		Bridge->RemoveFromRoot();
		Bridge = nullptr;
	}

	Bridge = NewObject<UCTRLStateTreeEventBridge>(Context.GetOwner(), NAME_None, RF_Transient);
	Bridge->AddToRoot();
	FStateTreeEventQueue& EventQueue = Context.GetMutableEventQueue();
	FString MsgPart = FString::Printf(TEXT("%s. Sending → StateTree %s"), *GetNameSafe(Actor), *GetNameSafe(Context.GetStateTree()));
	Bridge->EventReceived.BindWeakLambda(
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
	return Bridge;
}

EStateTreeRunStatus FCTRLGasEventToStateTreeEventTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	Data.Bridge = MakeBridge(Context);
	if (ListenForEvents(Context))
	{
		return EStateTreeRunStatus::Running;
	}
	return EStateTreeRunStatus::Failed;
}

bool FCTRLGasEventToStateTreeEventTask::ListenForEvents(FStateTreeExecutionContext const& Context) const
{
	UnlistenForEvents(Context);
	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	auto& [Actor, EventTags, bOnlyMatchExact, bOnlyTriggerOnce, Bridge, DelegateHandles] = Data;
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		if (Data.bOnlyMatchExact)
		{
			for (auto const& EventTag : EventTags)
			{
				if (!EventTag.IsValid()) continue;
				auto const DelegateHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EventTag).AddUObject(Bridge, &UCTRLStateTreeEventBridge::GameplayEventCallback);
				if (DelegateHandle.IsValid())
				{
					DelegateHandles.Add(DelegateHandle);
				}
			}
		}
		else
		{
			auto const DelegateHandle = ASC->AddGameplayEventTagContainerDelegate(
				EventTags,
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(Bridge, &UCTRLStateTreeEventBridge::GameplayEventContainerCallback)
			);
			if (DelegateHandle.IsValid())
			{
				DelegateHandles.Add(DelegateHandle);
			}
		}
		return true;
	}
	return false;
}

void FCTRLGasEventToStateTreeEventTask::UnlistenForEvents(FStateTreeExecutionContext const& Context) const
{
	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	auto& [Actor, EventTags, bOnlyMatchExact, bOnlyTriggerOnce, Bridge, DelegateHandles] = Data;
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor))
	{
		if (Data.bOnlyMatchExact)
		{
			for (auto const& EventTag : EventTags)
			{
				for (auto const& DelegateHandle : DelegateHandles)
				{
					if (!DelegateHandle.IsValid()) continue;
					if (ASC->GenericGameplayEventCallbacks.Contains(EventTag))
					{
						ASC->GenericGameplayEventCallbacks.FindRef(EventTag).Remove(DelegateHandle);
					}
				}
			}
		}
		else
		{
			for (auto const& DelegateHandle : DelegateHandles)
			{
				if (!DelegateHandle.IsValid()) continue;
				ASC->RemoveGameplayEventTagContainerDelegate(EventTags, DelegateHandle);
			}
		}
	}
	DelegateHandles.Reset();
}

void FCTRLGasEventToStateTreeEventTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	UnlistenForEvents(Context);
	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	if (auto const Bridge = Data.Bridge)
	{
		Bridge->EventReceived.Unbind();
		Bridge->RemoveFromRoot();
		Data.Bridge = nullptr;
	}
}
#if WITH_EDITOR
FText FCTRLGasEventToStateTreeEventTask::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const ActorName = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Actor)), Formatting).ToString();
	auto const TagNames = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, EventTags)), Formatting).ToString();
	return UCTRLStateTreeUtils::FormatDescription(
		FString::Printf(TEXT("%s<s>Gameplay → StateTree Event:</s> %s <s>on</s> %s"), *UCTRLStateTreeUtils::SymbolTaskContinuous, *TagNames, *ActorName),
		Formatting
	);
}
#endif

#undef LOCTEXT_NAMESPACE
