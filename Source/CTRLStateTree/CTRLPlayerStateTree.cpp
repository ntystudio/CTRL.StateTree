// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLPlayerStateTree.h"

#include "StateTreeExecutionContext.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "CTRLStateTree/CTRLStateTree.h"
#include "CTRLStateTree/CTRLPlayerStateTreeSchema.h"

#include "VisualLogger/VisualLogger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLPlayerStateTree)

TSubclassOf<UStateTreeSchema> UCTRLPlayerStateTreeComponent::GetSchema() const
{
	return UCTRLPlayerStateTreeSchema::StaticClass();
}

bool UCTRLPlayerStateTreeComponent::ValidateSchema(FStateTreeExecutionContext const& StateTreeContext) const
{
	// Ensure that the actor and smart object match the schema.
	auto const* Schema = Cast<UCTRLPlayerStateTreeSchema>(StateTreeContext.GetStateTree()->GetSchema());
	if (!IsValid(Schema))
	{
		UE_VLOG_UELOG(
			ControllerActor,
			LogCTRLStateTree,
			Error,
			TEXT("Failed to setup PlayerStateTree for %s. Expecting %s schema for StateTree asset: %s."),
			*GetNameSafe(ControllerActor),
			*GetNameSafe(UCTRLPlayerStateTreeSchema::StaticClass()),
			*GetFullNameSafe(StateTreeContext.GetStateTree())
		);

		return false;
	}
	if (!IsValid(ControllerActor) || !ControllerActor->IsA(Schema->GetControllerActorClass()))
	{
		UE_VLOG_UELOG(
			ControllerActor,
			LogCTRLStateTree,
			Error,
			TEXT("Failed to setup PlayerStateTree for %s. Expecting Controller to be of type %s (found %s, Valid: %s) for StateTree asset: %s."),
			*GetNameSafe(ControllerActor),
			*GetNameSafe(Schema->GetControllerActorClass()),
			*GetNameSafe(ControllerActor ? ControllerActor->GetClass() : nullptr),
			*LexToString(IsValid(ControllerActor)),
			*GetFullNameSafe(StateTreeContext.GetStateTree())
		);

		return false;
	}

	if (!IsValid(PawnActor) || !PawnActor->IsA(Schema->GetPawnActorClass()))
	{
		UE_VLOG_UELOG(
			PawnActor,
			LogCTRLStateTree,
			Error,
			TEXT("Failed to setup PlayerStateTree for %s. Expecting Pawn to be of type %s (found %s, Valid: %s) for StateTree asset: %s."),
			*GetNameSafe(PawnActor),
			*GetNameSafe(Schema->GetPawnActorClass()),
			*GetNameSafe(PawnActor ? PawnActor->GetClass() : nullptr),
			*LexToString(IsValid(PawnActor)),
			*GetFullNameSafe(StateTreeContext.GetStateTree())
		);

		return false;
	}

	return true;
}

void UCTRLPlayerStateTreeComponent::SetPlayerController_Implementation(APlayerController* InControllerActor)
{
	ControllerActor = InControllerActor;
	if (!ControllerActor)
	{
		PawnActor = nullptr;
		return;
	}
	PawnActor = ControllerActor->GetPawn();
}

void UCTRLPlayerStateTreeComponent::BeginPlay()
{
	Super::BeginPlay();
	SetPlayerController(ControllerActor);
}

bool UCTRLPlayerStateTreeComponent::SetContextRequirements(FStateTreeExecutionContext& StateTreeContext, bool bLogErrors)
{
	if (!IsValid(this))
	{
		// CTRLST_LOG(Warning, TEXT("Failed to set context requirements. Context is not properly setup."));
		return false;
	}

	if (!StateTreeContext.IsValid())
	{
		// CTRLST_LOG(Warning, TEXT("Failed to set context requirements. StateTreeContext is not valid."));
		CTRLST_LOG(Warning, TEXT("Failed to set context requirements. StateTreeContext is not valid."));
		return false;
	}
	if (!ValidateSchema(StateTreeContext)) { return false; }
	if (!ControllerActor)
	{
		CTRLST_LOG(Warning, TEXT("Failed to set context requirements. ControllerActor is not valid."));
		return false;
		// auto OuterController = GetTypedOuter<APlayerController>();
		// if (OuterController && !OuterController->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
		// {
		// 	ControllerActor = OuterController;
		// }
	}
	if (!PawnActor)
	{
		CTRLST_LOG(Warning, TEXT("Failed to set context requirements. PawnActor is not valid."));
		// PawnActor = ControllerActor ? ControllerActor->GetPawn() : nullptr;
		return false;
	}
	// CTRLST_LOG(Warning, TEXT("SetContextRequirements CardStacks UID: %d"), CardStacks->GetUniqueID());

	StateTreeContext.SetContextDataByName(UST::PlayerStateTree::Names::ControllerActor, FStateTreeDataView(ControllerActor));
	StateTreeContext.SetContextDataByName(UST::PlayerStateTree::Names::PawnActor, FStateTreeDataView(PawnActor));
	// auto StInstanceData = StateTreeContext.GetInstanceData();
	// CTRLST_LOG(Warning, TEXT("SetContextRequirements InstanceData CardStacks UID: %d"), StInstanceData->GetObject(2)->GetUniqueID());

	checkf(ControllerActor != nullptr, TEXT("Should never reach this point with an invalid ControllerActor since it is required to get a valid StateTreeContext."));
	UWorld const* World = ControllerActor->GetWorld();

	// StateTreeContext.SetCollectExternalDataCallback(
	// 	FOnCollectStateTreeExternalData::CreateWeakLambda(
	// 		this,
	// 		[World, this](
	// 		FStateTreeExecutionContext const& Context,
	// 		UStateTree const* StateTree,
	// 		TArrayView<FStateTreeExternalDataDesc const> ExternalDescs,
	// 		TArrayView<FStateTreeDataView> OutDataViews
	// 	)
	// 		{
	// 			check(ExternalDescs.Num() == OutDataViews.Num());
	// 			for (int32 Index = 0; Index < ExternalDescs.Num(); Index++)
	// 			{
	// 				FStateTreeExternalDataDesc const& Desc = ExternalDescs[Index];
	// 				if (Desc.Struct != nullptr)
	// 				{
	// 					if (World != nullptr && Desc.Struct->IsChildOf(UWorldSubsystem::StaticClass()))
	// 					{
	// 						UWorldSubsystem* Subsystem = World->GetSubsystemBase(Cast<UClass>(const_cast<UStruct*>(ToRawPtr(Desc.Struct))));
	// 						OutDataViews[Index] = FStateTreeDataView(Subsystem);
	// 					}
	// 					else if (Desc.Struct->IsChildOf(AController::StaticClass()))
	// 					{
	// 						OutDataViews[Index] = FStateTreeDataView(ControllerActor);
	// 					}
	// 					else if (Desc.Struct->IsChildOf(APawn::StaticClass()))
	// 					{
	// 						OutDataViews[Index] = FStateTreeDataView(PawnActor);
	// 					}
	// 				}
	// 			}
	//
	// 			return true;
	// 		}
	// 	)
	// );

	auto const Valid = StateTreeContext.AreContextDataViewsValid();
	if (!Valid)
	{
		CTRLST_LOG(Warning, TEXT("%s Failed to set context requirements. Context data views are not valid."), *GetNameSafe(this));
	}
	return Valid;
}
