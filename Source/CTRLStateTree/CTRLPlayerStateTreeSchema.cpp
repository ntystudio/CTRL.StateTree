// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLPlayerStateTreeSchema.h"

#include "StateTreeExecutionContext.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

#include "Subsystems/WorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLPlayerStateTreeSchema)

UCTRLPlayerStateTreeSchema::UCTRLPlayerStateTreeSchema()
	: ControllerActorClass(AController::StaticClass()),
	PawnActorClass(APawn::StaticClass()),
	ContextDataDescs(
		{
			{UST::PlayerStateTree::Names::ControllerActor, AController::StaticClass(), FGuid(0x89926642, 0x4C2EC33E, 0xE72E4296, 0x45D15029)},
			{UST::PlayerStateTree::Names::PawnActor, APawn::StaticClass(), FGuid(0xDFB93B9E, 0xEDBE4906, 0x851C66B2, 0x7585FA21)},
		}
	) {}

UClass* UCTRLPlayerStateTreeSchema::GetControllerActorClass() const
{
	return ControllerActorClass;
}

UClass* UCTRLPlayerStateTreeSchema::GetPawnActorClass() const
{
	return PawnActorClass;
}

TConstArrayView<FStateTreeExternalDataDesc> UCTRLPlayerStateTreeSchema::GetContextDataDescs() const
{
	return ContextDataDescs;
}

bool UCTRLPlayerStateTreeSchema::IsStructAllowed(UScriptStruct const* InScriptStruct) const
{
	// MT_LOG(Warning, TEXT("IsStructAllowed %s"), *InScriptStruct->GetName());
	return true;
	// return InScriptStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FGameplayInteractionStateTreeCondition::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FStateTreeEvaluatorCommonBase::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FStateTreeTaskCommonBase::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FGameplayInteractionStateTreeTask::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FStateTreeConsiderationCommonBase::StaticStruct());
}

bool UCTRLPlayerStateTreeSchema::IsClassAllowed(UClass const* InClass) const
{
	return IsChildOfBlueprintBase(InClass);
}

bool UCTRLPlayerStateTreeSchema::IsExternalItemAllowed(UStruct const& InStruct) const
{
	return InStruct.IsChildOf(AActor::StaticClass())
		|| InStruct.IsChildOf(UActorComponent::StaticClass())
		|| InStruct.IsChildOf(UWorldSubsystem::StaticClass());
}

void UCTRLPlayerStateTreeSchema::PostLoad()
{
	Super::PostLoad();
	ContextDataDescs[0].Struct = ControllerActorClass.Get();
	ContextDataDescs[1].Struct = PawnActorClass.Get();
}

#if WITH_EDITOR
void UCTRLPlayerStateTreeSchema::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	FProperty const* Property = PropertyChangedEvent.Property;

	if (Property)
	{
		if (Property->GetOwnerClass() == StaticClass()
			&& Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCTRLPlayerStateTreeSchema, ControllerActorClass))
		{
			ContextDataDescs[0].Struct = ControllerActorClass.Get();
		}
		if (Property->GetOwnerClass() == StaticClass()
			&& Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCTRLPlayerStateTreeSchema, PawnActorClass))
		{
			ContextDataDescs[1].Struct = PawnActorClass.Get();
		}
	}
}
#endif

