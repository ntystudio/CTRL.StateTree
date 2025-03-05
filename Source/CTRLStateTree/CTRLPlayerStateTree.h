// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeSchema.h"

#include "Components/StateTreeComponent.h"

#include "CTRLPlayerStateTree.generated.h"

UCLASS(DisplayName="Player StateTree Component [CTRL]", meta=(BlueprintSpawnableComponent))
class CTRLSTATETREE_API UCTRLPlayerStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	UCTRLPlayerStateTreeComponent()
	{
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
	void SetPlayerController(APlayerController* InControllerActor);
	virtual void SetPlayerController_Implementation(APlayerController* InControllerActor);

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Tree")
	TObjectPtr<APlayerController> ControllerActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State Tree")
	TObjectPtr<APawn> PawnActor;

	virtual bool SetContextRequirements(FStateTreeExecutionContext& StateTreeContext, bool bLogErrors) override;
};
