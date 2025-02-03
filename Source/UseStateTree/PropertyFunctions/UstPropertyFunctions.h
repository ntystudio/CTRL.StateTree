// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "UstPropertyFunctions.generated.h"

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Property Function Data ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ //

USTRUCT(meta = (Hidden))
struct USESTATETREE_API FUstIntCombinationPropertyFunctionInstanceData
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
struct USESTATETREE_API FUstSingleIntPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Input = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	int32 Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct USESTATETREE_API FUstSingleFloatPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	float Input = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	float Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct USESTATETREE_API FUstFloatCombinationPropertyFunctionInstanceData
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
struct USESTATETREE_API FUstObjectFunctionInstanceData
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
struct USESTATETREE_API FUstStringToTextFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FString Input = TEXT("");

	UPROPERTY(EditAnywhere, Category = Output)
	FText Result = FText::GetEmpty();
};

USTRUCT(meta = (Hidden))
struct USESTATETREE_API FUstStringToNameFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FString Input = TEXT("");

	UPROPERTY(EditAnywhere, Category = Output)
	FName Result = NAME_None;
};

USTRUCT(meta = (Hidden))
struct USESTATETREE_API FUstBoolToStringFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool Input = false;

	UPROPERTY(EditAnywhere, Category = Output)
	FString Result = TEXT("");
};

USTRUCT(meta=(Hidden, DisplayName = "Property Function Base [UST]"))
struct USESTATETREE_API FUstPropertyFunctionBase : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()
};

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Property Functions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ //

/*
 * Generate a random float between Left and Right
 */
USTRUCT(meta=(DisplayName = "Random Float [UST]", Category = "Math|Float"))
struct USESTATETREE_API FUstRandomFloatPropertyFunction : public FUstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUstFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Generate a random integer between Left and Right
 */
USTRUCT(meta=(DisplayName = "Random Integer [UST]", Category = "Math|Integer"))
struct USESTATETREE_API FUstRandomIntPropertyFunction : public FUstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUstFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert an FString to an FText
 */
USTRUCT(meta=(DisplayName = "String To Text [UST]", Category = "Object"))
struct USESTATETREE_API FUstStringToTextPropertyFunction : public FUstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUstStringToTextFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert an FString to an FName
 */
USTRUCT(meta=(DisplayName = "String To Name [UST]", Category = "Object"))
struct USESTATETREE_API FUstStringToNamePropertyFunction : public FUstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUstStringToNameFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert a bool to a string
 */
USTRUCT(meta=(DisplayName = "Bool To String [UST]", Category = "Object"))
struct USESTATETREE_API FUstBoolToStringPropertyFunction : public FUstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUstBoolToStringFunctionInstanceData;

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
USTRUCT(meta=(DisplayName = "Object Is Valid [UST]", Category = "Object"))
struct USESTATETREE_API FUstObjectIsValidPropertyFunction : public FUstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FUstObjectFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};