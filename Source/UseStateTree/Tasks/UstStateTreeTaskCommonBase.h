// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "UstStateTreeTaskCommonBase.generated.h"

/**
 * Base class (namespace) for all EST Tasks
 * This allows schemas to safely include all tasks child of this struct. 
 */
USTRUCT(BlueprintType, meta = (Hidden, Category="UST"))
struct FUstStateTreeTaskCommonBase : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FUstStateTreeTaskCommonBase()
	{
		// better defaults
		bShouldCallTick = false;
		bShouldStateChangeOnReselect = false;
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bDebugEnabled = false;

#if WITH_EDITOR
	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::DarkGreen;
	}
	virtual FName GetIconName() const override
	{
		return FName("StateTreeEditorStyle|Node.Text");
	}
#endif
};
