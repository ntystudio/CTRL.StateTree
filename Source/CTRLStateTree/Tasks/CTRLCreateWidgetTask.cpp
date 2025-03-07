// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLCreateWidgetTask.h"

#include "CommonActivatableWidget.h"

#include "Blueprint/UserWidget.h"

#include "CTRLStateTree/CTRLStateTreeUtils.h"

#include "Engine/Engine.h"

#include "VisualLogger/VisualLogger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLCreateWidgetTask)

#define LOCTEXT_NAMESPACE "FCTRLCreateWidgetTask"

struct FStateTreeExecutionContext;

UCTRLCreateWidgetTaskData::UCTRLCreateWidgetTaskData(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	WidgetTemplate = CreateDefaultSubobject<UUserWidget>(TEXT("WidgetTemplate"));
}

void UCTRLCreateWidgetTaskData::Destruct()
{
	if (auto const StateTreeSendEventSubsystem = UCTRLStateTreeWidgetEventSubsystem::Get(this))
	{
		StateTreeSendEventSubsystem->Unlisten(Widget);
	}
	if (bRemoveFromViewportOnExit && IsValid(Widget) && Widget->IsInViewport())
	{
		Widget->RemoveFromParent();
	}

	Widget = nullptr;
	ActivatableWidget = nullptr;
	CommonWidget = nullptr;
	ClearCachedInstanceData();
	if (OnDestruct.IsBound()) OnDestruct.Broadcast();
}

void FCTRLCreateWidgetTask::ExitState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<UInstanceDataType>(*this);
	InstanceData.Destruct();
	Super::ExitState(Context, Transition);
}

FText FCTRLCreateWidgetTask::GetDescription(FGuid const& ID, FStateTreeDataView const InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting const Formatting) const
{
	auto const InstanceData = InstanceDataView.GetPtr<UInstanceDataType>();
	FString const WidgetDesc = InstanceData->WidgetTemplate ? InstanceData->WidgetTemplate->GetClass()->GetDisplayNameText().ToString() : FString(TEXT("<b>Invalid Template</b>"));
	FString Desc = FString::Printf(TEXT("<s>Create Widget</s> <b>%s</b> "), *WidgetDesc);
	if (InstanceData->bAddToViewportOnEnter)
	{
		Desc += UCTRLStateTreeUtils::SymbolStateEnter + TEXT(" Add");
		if (InstanceData->ZIndex != 0)
		{
			Desc += FString::Printf(TEXT(" <s>Z-Index: %d</s>"), InstanceData->ZIndex);
		}
	}
	if (InstanceData->bRemoveFromViewportOnExit)
	{
		Desc += UCTRLStateTreeUtils::SymbolStateExit + TEXT(" Remove");
	}

	return UCTRLStateTreeUtils::FormatDescription(Desc, Formatting);
}

EStateTreeRunStatus FCTRLCreateWidgetTask::EnterState(FStateTreeExecutionContext& Context, FStateTreeTransitionResult const& Transition) const
{
	auto& InstanceData = Context.GetInstanceData<UInstanceDataType>(*this);
	InstanceData.SetCachedInstanceDataFromContext(Context);
	// SetCachedInstanceDataFromContext(Context);
	if (!bShouldStateChangeOnReselect && Transition.ChangeType == EStateTreeStateChangeType::Sustained)
	{
		// Unreal has fucked up if this happens, so just exit with what it was going to do anyway
		return EStateTreeRunStatus::Running;
	}

	if (!IsValid(InstanceData.TargetPlayerController))
	{
		CTRLST_LOG(Error, TEXT("TargetPlayerPawn is invalid %s"), *GetNameSafe(InstanceData.TargetPlayerController));
		return EStateTreeRunStatus::Failed;
	}
	if (!IsValid(InstanceData.WidgetTemplate))
	{
		CTRLST_LOG(Error, TEXT("WidgetTemplate is invalid %s"), *GetNameSafe(InstanceData.WidgetTemplate));
		return EStateTreeRunStatus::Failed;
	}

	if (InstanceData.Widget)
	{
		// this shouldn't happen, but just in case
		CTRLST_LOG(Warning, TEXT("Widget is already created %s"), *GetNameSafe(InstanceData.Widget));
		return EStateTreeRunStatus::Running;
	}

	bool const bSuccess = InstanceData.Setup();
	if (!bSuccess)
	{
		return EStateTreeRunStatus::Failed;
	}
	return Super::EnterState(Context, Transition);
}

bool UCTRLCreateWidgetTaskData::Setup()
{
	// Create the widget.
	Widget = DuplicateObject<UUserWidget>(WidgetTemplate, TargetPlayerController);
	if (!Widget)
	{
		CTRLST_LOG(Error, TEXT("Failed to duplicate WidgetTemplate %s"), *GetNameSafe(WidgetTemplate));
		return false;
	}
	// Listen for state tree events.
	if (auto const StateTreeSendEventSubsystem = UCTRLStateTreeWidgetEventSubsystem::Get(this))
	{
		StateTreeSendEventSubsystem->Listen(Widget, this);
	}

	Widget->SetFlags(RF_Transactional);
	Widget->SetOwningPlayer(TargetPlayerController);
	Widget->Initialize();
	ActivatableWidget = Cast<UCommonActivatableWidget>(Widget);
	CommonWidget = Cast<UCommonUserWidget>(Widget);

	if (bAddToViewportOnEnter)
	{
		Widget->AddToViewport(ZIndex);
	}
	return true;
}

void UCTRLCreateWidgetTaskData::SendEvent(FStateTreeEvent const& Event) const
{
	TSharedPtr<FStateTreeInstanceStorage> const InstanceStorage = WeakInstanceStorage.Pin();

	if (InstanceStorage == nullptr || CachedOwner == nullptr)
	{
		UE_VLOG_UELOG(this, LogStateTree, Error, TEXT("Trying to call SendEvent() while node is not active. Use SendEvent() on UStateTreeComponent instead for sending signals externally."));
		return;
	}
	InstanceStorage->GetMutableEventQueue().SendEvent(CachedOwner, Event.Tag, Event.Payload, Event.Origin);
}

void UCTRLCreateWidgetTaskData::SetCachedInstanceDataFromContext(FStateTreeExecutionContext const& Context) const
{
	if (FStateTreeInstanceData* InstanceData = Context.GetMutableInstanceData())
	{
		WeakInstanceStorage = InstanceData->GetWeakMutableStorage();
	}
	CachedState = Context.GetCurrentlyProcessedState();
	CachedOwner = Context.GetOwner();

	FStateTreeExecutionFrame const* CurrentlyProcessedFrame = Context.GetCurrentlyProcessedFrame();
	check(CurrentlyProcessedFrame);

	CachedFrameStateTree = CurrentlyProcessedFrame->StateTree;
	CachedFrameRootState = CurrentlyProcessedFrame->RootState;
}

void UCTRLCreateWidgetTaskData::ClearCachedInstanceData() const
{
	WeakInstanceStorage = nullptr;
	CachedState = FStateTreeStateHandle::Invalid;
	CachedOwner = nullptr;
	CachedFrameStateTree = nullptr;
	CachedFrameRootState = FStateTreeStateHandle::Invalid;
}

void UCTRLStateTreeWidgetEventSubsystem::Unlisten(UUserWidget const* Widget)
{
	if (!Widget) { return; }
	if (!WidgetToNodeMap.Contains(Widget)) { return; }
	WidgetToNodeMap.Remove(Widget);
	// PurgeStale();
}

UCTRLStateTreeWidgetEventSubsystem* UCTRLStateTreeWidgetEventSubsystem::Get(UObject const* WorldContextObject)
{
	auto const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) { return nullptr; }
	return World->GetSubsystem<UCTRLStateTreeWidgetEventSubsystem>();
}

bool UCTRLStateTreeWidgetEventSubsystem::K2_SendStateTreeEvent(UUserWidget* Widget, FStateTreeEvent const& StateTreeEvent)
{
	if (auto const StateTreeSendEventSubsystem = Get(Widget))
	{
		return StateTreeSendEventSubsystem->SendStateTreeEvent(Widget, StateTreeEvent);
	}
	return false;
}

bool UCTRLStateTreeWidgetEventSubsystem::SendStateTreeEvent(UUserWidget* Widget, FStateTreeEvent const& StateTreeEvent)
{
	if (!Widget) { return false; }
	if (!WidgetToNodeMap.Contains(Widget))
	{
		CTRLST_LOG(Warning, TEXT("Object %s is not listening for StateTree events."), *GetNameSafe(Widget));
		return false;
	}

	// ON_SCOPE_EXIT
	// {
	// 	PurgeStale();
	// };

	if (!IsValid(Widget))
	{
		CTRLST_LOG(Warning, TEXT("Widget is invalid"));
		return false;
	}

	auto const WeakNode = WidgetToNodeMap.FindRef(Widget);
	if (!WeakNode.IsValid())
	{
		CTRLST_LOG(Warning, TEXT("Node is invalid"));
		return false;
	}
	WeakNode->SendEvent(StateTreeEvent);
	return true;
}

void UCTRLStateTreeWidgetEventSubsystem::Listen(UUserWidget* Widget, UCTRLCreateWidgetTaskData* Node)
{
	if (!IsValid(Widget)) { return; }
	if (!IsValid(Node)) { return; }
	Widget->OnNativeDestruct.AddWeakLambda(
		Node,
		[this](UUserWidget* Widget)
		{
			WidgetToNodeMap.Remove(Widget);
		}
	);
	Node->OnDestruct.AddWeakLambda(
		Widget,
		[this, Widget]()
		{
			WidgetToNodeMap.Remove(Widget);
		}
	);
	WidgetToNodeMap.Add(Widget, Node);
	PurgeStale();
}

void UCTRLStateTreeWidgetEventSubsystem::PurgeStale()
{
	WidgetToNodeMap = WidgetToNodeMap.FilterByPredicate(
		[](TPair<TWeakObjectPtr<UUserWidget>, TWeakObjectPtr<UCTRLCreateWidgetTaskData>> const KV)
		{
			bool bValid = true;
			if (!KV.Key.IsValid())
			{
				CTRLST_LOG(Warning, TEXT("WidgetToNodeMap contains invalid Object"));
				bValid = false;
			}
			if (!KV.Value.IsValid())
			{
				CTRLST_LOG(Warning, TEXT("WidgetToNodeMap contains invalid Node"));
				bValid = false;
			}
			return bValid;
		}
	);
}

#undef LOCTEXT_NAMESPACE
