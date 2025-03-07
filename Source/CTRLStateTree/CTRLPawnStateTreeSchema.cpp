// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLPawnStateTreeSchema.h"

#include "StateTreeExecutionContext.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

#include "Subsystems/WorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLPawnStateTreeSchema)

UCTRLPawnStateTreeSchema::UCTRLPawnStateTreeSchema():
	ControllerActorClass(AController::StaticClass()),
	PawnActorClass(APawn::StaticClass()),
	OwnerActorClass(AActor::StaticClass()),
	ContextDataDescs(
		{
			{UST::PawnStateTree::Names::ControllerActor, AController::StaticClass(), FGuid(0x89926642, 0x4C2EC33E, 0xE72E4296, 0x45D15029)},
			{UST::PawnStateTree::Names::PawnActor, APawn::StaticClass(), FGuid(0xDFB93B9E, 0xEDBE4906, 0x851C66B2, 0x7585FA21)},
			{UST::PawnStateTree::Names::OwnerActor, AActor::StaticClass(), FGuid(0xFAF5E70D, 0x4EF92F9F, 0xE6172D9D, 0xF71BFA47)},
		}
	) {}
UClass* UCTRLPawnStateTreeSchema::GetControllerActorClass() const
{
	return ControllerActorClass;
}

UClass* UCTRLPawnStateTreeSchema::GetPawnActorClass() const
{
	return PawnActorClass;
}

UClass* UCTRLPawnStateTreeSchema::GetOwnerActorClass() const
{
	return OwnerActorClass;
}

TConstArrayView<FStateTreeExternalDataDesc> UCTRLPawnStateTreeSchema::GetContextDataDescs() const
{
	return ContextDataDescs;
}

bool UCTRLPawnStateTreeSchema::IsStructAllowed(UScriptStruct const* InScriptStruct) const
{
	// CTRLST_LOG(Warning, TEXT("IsStructAllowed %s"), *InScriptStruct->GetName());
	return true;
	// return InScriptStruct->IsChildOf(FStateTreeConditionCommonBase::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FGameplayInteractionStateTreeCondition::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FStateTreeEvaluatorCommonBase::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FStateTreeTaskCommonBase::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FGameplayInteractionStateTreeTask::StaticStruct())
	// 	|| InScriptStruct->IsChildOf(FStateTreeConsiderationCommonBase::StaticStruct());
}

bool UCTRLPawnStateTreeSchema::IsClassAllowed(UClass const* InClass) const
{
	return IsChildOfBlueprintBase(InClass);
}

bool UCTRLPawnStateTreeSchema::IsExternalItemAllowed(UStruct const& InStruct) const
{
	return InStruct.IsChildOf(AActor::StaticClass())
		|| InStruct.IsChildOf(UActorComponent::StaticClass())
		|| InStruct.IsChildOf(UWorldSubsystem::StaticClass());
}

void UCTRLPawnStateTreeSchema::PostLoad()
{
	Super::PostLoad();
	ContextDataDescs[0].Struct = ControllerActorClass.Get();
	ContextDataDescs[1].Struct = PawnActorClass.Get();
	ContextDataDescs[2].Struct = OwnerActorClass.Get();
}

#if WITH_EDITOR
void UCTRLPawnStateTreeSchema::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (FProperty const* Property = PropertyChangedEvent.Property)
	{
		if (Property->GetOwnerClass() == StaticClass()
			&& Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCTRLPawnStateTreeSchema, ControllerActorClass))
		{
			ContextDataDescs[0].Struct = ControllerActorClass.Get();
		}
		if (Property->GetOwnerClass() == StaticClass()
			&& Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCTRLPawnStateTreeSchema, PawnActorClass))
		{
			ContextDataDescs[1].Struct = PawnActorClass.Get();
		}
		if (Property->GetOwnerClass() == StaticClass()
			&& Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCTRLPawnStateTreeSchema, OwnerActorClass))
		{
			ContextDataDescs[2].Struct = OwnerActorClass.Get();
		}
	}
}
#endif
