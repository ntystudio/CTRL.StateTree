// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "EstStateTreeTaskCommonBase.generated.h"

/**
 * Base class (namespace) for all EST Tasks
 * This allows schemas to safely include all tasks child of this struct. 
 */
USTRUCT(BlueprintType, meta = (Hidden))
struct FEstStateTreeTaskCommonBase : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FEstStateTreeTaskCommonBase()
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
		return UE::StateTree::Colors::Grey;
	}
	virtual FName GetIconName() const override
	{
		return FName("StateTreeEditorStyle|Node.Text");
	}
#endif
};
