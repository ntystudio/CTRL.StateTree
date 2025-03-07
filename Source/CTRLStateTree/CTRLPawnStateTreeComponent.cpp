// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLPawnStateTreeComponent.h"

#include "StateTreeExecutionContext.h"

#include "CTRLStateTree/CTRLPawnStateTreeSchema.h"
#include "CTRLStateTree/CTRLStateTree.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "VisualLogger/VisualLogger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLPawnStateTreeComponent)

TSubclassOf<UStateTreeSchema> UCTRLPawnStateTreeComponent::GetSchema() const
{
	return UCTRLPawnStateTreeSchema::StaticClass();
}

bool UCTRLPawnStateTreeComponent::ValidateSchema(FStateTreeExecutionContext const& StateTreeContext) const
{
	// Ensure that the actor and smart object match the schema.
	auto const* Schema = Cast<UCTRLPawnStateTreeSchema>(StateTreeContext.GetStateTree()->GetSchema());
	if (!IsValid(Schema))
	{
		UE_VLOG_UELOG(
			ControllerActor,
			LogCTRLStateTree,
			Error,
			TEXT("Failed to setup PawnStateTree for %s. Expecting %s schema for StateTree asset: %s."),
			*GetNameSafe(ControllerActor),
			*GetNameSafe(UCTRLPawnStateTreeSchema::StaticClass()),
			*GetFullNameSafe(StateTreeContext.GetStateTree())
		);

		return false;
	}

	if (!IsValid(OwnerActor) || !OwnerActor->IsA(Schema->GetOwnerActorClass()))
	{
		UE_VLOG_UELOG(
			OwnerActor,
			LogCTRLStateTree,
			Error,
			TEXT("Failed to setup PawnStateTree for %s. Expecting Owner to be of type %s (found %s, Valid: %s) for StateTree asset: %s."),
			*GetNameSafe(OwnerActor),
			*GetNameSafe(Schema->GetOwnerActorClass()),
			*GetNameSafe(OwnerActor ? OwnerActor->GetClass() : nullptr),
			*LexToString(IsValid(OwnerActor)),
			*GetFullNameSafe(StateTreeContext.GetStateTree())
		);

		return false;
	}

	bool bIsValidSchema = true;

	if (!IsValid(ControllerActor) || !ControllerActor->IsA(Schema->GetControllerActorClass()))
	{
		if (bRequiresController || IsValid(ControllerActor))
		{
			UE_VLOG_UELOG(
				OwnerActor,
				LogCTRLStateTree,
				Error,
				TEXT("Failed to setup PawnStateTree for %s. Expecting Controller to be a %s (Found %s, IsValid: %s, IsRequired: %s) for StateTree asset: %s."),
				*GetNameSafe(ControllerActor),
				*GetNameSafe(Schema->GetControllerActorClass()),
				*GetNameSafe(ControllerActor ? ControllerActor->GetClass() : nullptr),
				*LexToString(IsValid(ControllerActor)),
				*LexToString(bRequiresController),
				*GetFullNameSafe(StateTreeContext.GetStateTree())
			);
			bIsValidSchema = false;
		}
	}

	if (!IsValid(PawnActor) || !PawnActor->IsA(Schema->GetPawnActorClass()))
	{
		if (bRequiresPawn || IsValid(PawnActor))
		{
			UE_VLOG_UELOG(
				OwnerActor,
				LogCTRLStateTree,
				Error,
				TEXT("Failed to setup PawnStateTree for %s. Expecting Pawn to be a %s (Found %s, IsValid: %s, IsRequired: %s) for StateTree asset: %s."),
				*GetNameSafe(PawnActor),
				*GetNameSafe(Schema->GetPawnActorClass()),
				*GetNameSafe(PawnActor ? PawnActor->GetClass() : nullptr),
				*LexToString(IsValid(PawnActor)),
				*LexToString(bRequiresPawn),
				*GetFullNameSafe(StateTreeContext.GetStateTree())
			);
			bIsValidSchema = false;
		}

		return bIsValidSchema;
	}

	return true;
}

void UCTRLPawnStateTreeComponent::SetController_Implementation(AController* InControllerActor)
{
	if (bAutoSetupPawnControllerFromOwner)
	{
		CTRLST_LOG(Warning, TEXT("SetController_Implementation called with bAutoSetupPawnControllerFromOwner set to true. This should not happen."));
	}
	ControllerActor = InControllerActor;
	if (!ControllerActor)
	{
		PawnActor = nullptr;
		return;
	}
	PawnActor = ControllerActor->GetPawn();
}

void UCTRLPawnStateTreeComponent::AssignContextActors()
{
	if (!IsValid(this))
	{
		return;
	}

	if (!IsValid(OwnerActor))
	{
		OwnerActor = GetOwner();
		if (!IsValid(OwnerActor))
		{
			OwnerActor = GetTypedOuter<AActor>();
		}
	}

	if (bAutoSetupPawnControllerFromOwner)
	{
		if (!IsValid(ControllerActor))
		{
			SetController(Cast<AController>(OwnerActor));
			if (auto const Controller = Cast<AController>(OwnerActor))
			{
				ControllerActor = Controller;
				PawnActor = Controller->GetPawn();
			}
		}
		if (!IsValid(PawnActor))
		{
			if (auto const Pawn = Cast<APawn>(OwnerActor))
			{
				PawnActor = Pawn;
				ControllerActor = Pawn->GetController();
			}
		}
	}
}

void UCTRLPawnStateTreeComponent::StartLogic()
{
	Super::StartLogic();
}

void UCTRLPawnStateTreeComponent::BeginPlay()
{
	Super::BeginPlay();
	AssignContextActors();
}

bool UCTRLPawnStateTreeComponent::SetContextRequirements(FStateTreeExecutionContext& StateTreeContext, bool bLogErrors)
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

	AssignContextActors();
	if (!OwnerActor)
	{
		OwnerActor = StateTreeContext.GetOwner();
		AssignContextActors();
	}

	if (!ValidateSchema(StateTreeContext)) { return false; }

	if (!StateTreeContext.SetContextDataByName(UST::PawnStateTree::Names::OwnerActor, FStateTreeDataView(OwnerActor)))
	{
		CTRLST_LOG(Warning, TEXT("Failed to set context requirements. OwnerActor is not valid."));
		return false;
	}

	if (!StateTreeContext.SetContextDataByName(UST::PawnStateTree::Names::ControllerActor, FStateTreeDataView(ControllerActor)))
	{
		if (bRequiresController)
		{
			CTRLST_LOG(Warning, TEXT("Failed to set context requirements. ControllerActor is not valid."));
			// return false;
			// auto OuterController = GetTypedOuter<APlayerController>();
			// if (OuterController && !OuterController->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject))
			// {
			// 	ControllerActor = OuterController;
			// }
		}
	}

	if (!StateTreeContext.SetContextDataByName(UST::PawnStateTree::Names::PawnActor, FStateTreeDataView(PawnActor)))
	{
		if (bRequiresPawn)
		{
			CTRLST_LOG(Warning, TEXT("Failed to set context requirements. PawnActor is not valid."));
			// PawnActor = ControllerActor ? ControllerActor->GetPawn() : nullptr;
			return false;
		}
	}

	// StateTreeContext.SetContextDataByName(UST::PawnStateTree::Names::ControllerActor, FStateTreeDataView(ControllerActor));
	// StateTreeContext.SetContextDataByName(UST::PawnStateTree::Names::PawnActor, FStateTreeDataView(PawnActor));
	// StateTreeContext.SetContextDataByName(UST::PawnStateTree::Names::OwnerActor, FStateTreeDataView(OwnerActor));
	// auto StInstanceData = StateTreeContext.GetInstanceData();
	// CTRLST_LOG(Warning, TEXT("SetContextRequirements InstanceData CardStacks UID: %d"), StInstanceData->GetObject(2)->GetUniqueID());

	checkf(OwnerActor != nullptr, TEXT("Should never reach this point with an invalid OwnerActor since it is required to get a valid StateTreeContext."));
	UWorld const* World = OwnerActor->GetWorld();

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

	return true;
	// auto const Valid = StateTreeContext.AreContextDataViewsValid();
	// if (!Valid)
	// {
	// 	CTRLST_LOG(Warning, TEXT("%s Failed to set context requirements. Context data views are not valid."), *GetNameSafe(this));
	// }
	// return Valid;
}
