// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeSchema.h"

#include "Components/StateTreeComponent.h"

#include "UObject/Object.h"

#include "CTRLPawnStateTreeSchema.generated.h"

namespace CTRL::PawnStateTree::Names
{
	FName const ControllerActor = TEXT("ControllerActor");
	FName const PawnActor = TEXT("PawnActor");
	FName const OwnerActor = TEXT("OwnerActor");
}

UCLASS(BlueprintType, EditInlineNew, CollapseCategories, meta = (DisplayName = "Pawn State Tree Schema [CTRL]", CommonSchema))
class CTRLSTATETREE_API UCTRLPawnStateTreeSchema : public UStateTreeSchema
{
	GENERATED_BODY()

public:
	UCTRLPawnStateTreeSchema();

	UClass* GetControllerActorClass() const;
	UClass* GetPawnActorClass() const;
	UClass* GetOwnerActorClass() const;

protected:
	virtual bool IsStructAllowed(UScriptStruct const* InScriptStruct) const override;
	virtual bool IsClassAllowed(UClass const* InScriptStruct) const override;
	virtual bool IsExternalItemAllowed(UStruct const& InStruct) const override;

	virtual TConstArrayView<FStateTreeExternalDataDesc> GetContextDataDescs() const override;

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	/** Actor class the StateTree is expected to run on. Allows to bind to specific Actor class' properties. */
	UPROPERTY(EditAnywhere, Category="Defaults")
	TSubclassOf<AController> ControllerActorClass;

	/** Actor class the StateTree is expected to run on. Allows to bind to specific Actor class' properties. */
	UPROPERTY(EditAnywhere, Category="Defaults")
	TSubclassOf<APawn> PawnActorClass;

	/** Actor class the StateTree is expected to run on. Allows to bind to specific Actor class' properties. */
	UPROPERTY(EditAnywhere, Category="Defaults")
	TSubclassOf<AActor> OwnerActorClass;

	/** List of named external data required by schema and provided to the state tree through the execution context. */
	UPROPERTY()
	TArray<FStateTreeExternalDataDesc> ContextDataDescs;
};
