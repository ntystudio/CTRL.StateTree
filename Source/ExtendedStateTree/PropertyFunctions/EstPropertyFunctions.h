// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "EstPropertyFunctions.generated.h"

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Property Function Data ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ //

USTRUCT(meta = (Hidden))
struct EXTENDEDSTATETREE_API FEstIntCombinationPropertyFunctionInstanceData
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
struct EXTENDEDSTATETREE_API FEstSingleIntPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	int32 Input = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	int32 Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct EXTENDEDSTATETREE_API FEstSingleFloatPropertyFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	float Input = 1.f;

	UPROPERTY(EditAnywhere, Category = Output)
	float Result = 0.f;
};

USTRUCT(meta = (Hidden))
struct EXTENDEDSTATETREE_API FEstFloatCombinationPropertyFunctionInstanceData
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
struct EXTENDEDSTATETREE_API FEstObjectFunctionInstanceData
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
struct EXTENDEDSTATETREE_API FEstStringToTextFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FString Input = TEXT("");

	UPROPERTY(EditAnywhere, Category = Output)
	FText Result = FText::GetEmpty();
};

USTRUCT(meta = (Hidden))
struct EXTENDEDSTATETREE_API FEstStringToNameFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	FString Input = TEXT("");

	UPROPERTY(EditAnywhere, Category = Output)
	FName Result = NAME_None;
};

USTRUCT(meta = (Hidden))
struct EXTENDEDSTATETREE_API FEstBoolToStringFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool Input = false;

	UPROPERTY(EditAnywhere, Category = Output)
	FString Result = TEXT("");
};

USTRUCT(meta=(Hidden, DisplayName = "Property Function Base [EST]"))
struct EXTENDEDSTATETREE_API FEstPropertyFunctionBase : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()
};

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ Property Functions ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ //

/*
 * Generate a random float between Left and Right
 */
USTRUCT(meta=(DisplayName = "Random Float [EST]", Category = "Math|Float"))
struct EXTENDEDSTATETREE_API FEstRandomFloatPropertyFunction : public FEstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Generate a random integer between Left and Right
 */
USTRUCT(meta=(DisplayName = "Random Integer [EST]", Category = "Math|Integer"))
struct EXTENDEDSTATETREE_API FEstRandomIntPropertyFunction : public FEstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert an FString to an FText
 */
USTRUCT(meta=(DisplayName = "String To Text [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstStringToTextPropertyFunction : public FEstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstStringToTextFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert an FString to an FName
 */
USTRUCT(meta=(DisplayName = "String To Name [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstStringToNamePropertyFunction : public FEstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstStringToNameFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

/*
 * Convert a bool to a string
 */
USTRUCT(meta=(DisplayName = "Bool To String [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstBoolToStringPropertyFunction : public FEstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstBoolToStringFunctionInstanceData;

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
USTRUCT(meta=(DisplayName = "Object Is Valid [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstObjectIsValidPropertyFunction : public FEstPropertyFunctionBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstObjectFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};