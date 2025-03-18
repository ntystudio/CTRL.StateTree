// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeSchema.h"

#include "Components/StateTreeComponent.h"

#include "CTRLPawnStateTreeComponent.generated.h"

UCLASS(DisplayName="Pawn StateTree Component [CTRL]", meta=(BlueprintSpawnableComponent))
class CTRLSTATETREE_API UCTRLPawnStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	UCTRLPawnStateTreeComponent()
	{
		OwnerActor = nullptr;
		ControllerActor = nullptr;
		PawnActor = nullptr;
	}

	virtual TSubclassOf<UStateTreeSchema> GetSchema() const override;

	FStateTreeReference const& GetStateTreeReference() const
	{
		return StateTreeRef;
	}

	virtual bool ValidateSchema(FStateTreeExecutionContext const& StateTreeContext) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="State Tree")
	void SetController(AController* InControllerActor);
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	virtual void SetController_Implementation(AController* InControllerActor);
	void AssignContextActors();
	virtual void StartLogic() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Tree")
	TObjectPtr<AController> ControllerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Tree")
	TObjectPtr<APawn> PawnActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Tree")
	TObjectPtr<UObject> OwnerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Tree")
	bool bRequiresController = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Tree")
	bool bRequiresPawn = true;

	// if true, the component will try to set the controller and pawn from the owner actor (if it is a controller or pawn)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Tree")
	bool bAutoSetupPawnControllerFromOwner = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Tree")
	bool bRestartLogicOnPossessedPawnChanged = true;

	virtual bool SetContextRequirements(FStateTreeExecutionContext& StateTreeContext, bool bLogErrors) override;
};
