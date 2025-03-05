// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "CTRLStateTreeCommonBaseTask.generated.h"

/**
 * Base class (namespace) for all EST Tasks
 * This allows schemas to safely include all tasks child of this struct. 
 */
USTRUCT(BlueprintType, meta = (Hidden, Category="UST"))
struct FCTRLStateTreeCommonBaseTask : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	FCTRLStateTreeCommonBaseTask()
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
