// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "CTRLPropertyFunctions.generated.h"

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Property Function Data ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ //

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLIntCombinationPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Left = 0;

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Right = 0;

	UPROPERTY(EditAnywhere, Category = Output)
	int32 Result = 0;
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLSingleIntPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Input = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	int32 Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLSingleFloatPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	float Input = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	float Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLFloatCombinationPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	float Left = 0.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float Right = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	float Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLObjectFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	TObjectPtr<UObject> Input = nullptr;

	UPROPERTY(EditAnywhere, Category = Output)
	bool Result = false;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bInvert = false;
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLStringToTextFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FString Input = TEXT("");

	UPROPERTY(EditAnywhere, Category = Output)
	FText Result = FText::GetEmpty();
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLStringToNameFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FString Input = TEXT("");

	UPROPERTY(EditAnywhere, Category = Output)
	FName Result = NAME_None;
};

USTRUCT(meta = (Hidden))
struct CTRLSTATETREE_API FCTRLBoolToStringFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool Input = false;

	UPROPERTY(EditAnywhere, Category = Output)
	FString Result = TEXT("");
};

USTRUCT(meta=(Hidden, DisplayName = "Property Function Base [CTRL]"))
struct CTRLSTATETREE_API FCTRLPropertyFunctionBase : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()
};

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Property Functions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ //

/*
 * Generate a random float between Left and Right
 */
USTRUCT(meta=(DisplayName = "Random Float [CTRL]", Category = "Math|Float"))
struct CTRLSTATETREE_API FCTRLRandomFloatPropertyFunction : public FCTRLPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Generate a random integer between Left and Right
 */
USTRUCT(meta=(DisplayName = "Random Integer [CTRL]", Category = "Math|Integer"))
struct CTRLSTATETREE_API FCTRLRandomIntPropertyFunction : public FCTRLPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert an FString to an FText
 */
USTRUCT(meta=(DisplayName = "String To Text [CTRL]", Category = "Object"))
struct CTRLSTATETREE_API FCTRLStringToTextPropertyFunction : public FCTRLPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLStringToTextFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert an FString to an FName
 */
USTRUCT(meta=(DisplayName = "String To Name [CTRL]", Category = "Object"))
struct CTRLSTATETREE_API FCTRLStringToNamePropertyFunction : public FCTRLPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLStringToNameFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert a bool to a string
 */
USTRUCT(meta=(DisplayName = "Bool To String [CTRL]", Category = "Object"))
struct CTRLSTATETREE_API FCTRLBoolToStringPropertyFunction : public FCTRLPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLBoolToStringFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Check if an object is valid
 * Same as built-in ObjectIsValid, but with fixed description when inverted
 */
USTRUCT(meta=(DisplayName = "Object Is Valid [CTRL]", Category = "Object"))
struct CTRLSTATETREE_API FCTRLObjectIsValidPropertyFunction : public FCTRLPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FCTRLObjectFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};