// SPDX-FileCopyrightText: 2025 NTY.studio

#pragma once

#include "CoreMinimal.h"
#include "StateTreePropertyFunctionBase.h"
#include "EstPropertyFunctions.generated.h"

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
struct EXTENDEDSTATETREE_API FEstBoolToStringFunctionInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool Input = false;

	UPROPERTY(EditAnywhere, Category = Output)
	FString Result = TEXT("");
};

/**
 * 
 */
USTRUCT(meta=(DisplayName = "Random Float [EST]", Category = "Math|Float"))
struct EXTENDEDSTATETREE_API FEstRandomFloatPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

USTRUCT(meta=(DisplayName = "Random Integer [EST]", Category = "Math|Integer"))
struct EXTENDEDSTATETREE_API FEstRandomIntPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstFloatCombinationPropertyFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

USTRUCT(meta=(DisplayName = "String To Text [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstStringToTextPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstStringToTextFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

USTRUCT(meta=(DisplayName = "Bool To String [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstBoolToStringPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstBoolToStringFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};

// Same as built-in ObjectIsValid, but with fixed description when inverted
USTRUCT(meta=(DisplayName = "Object Is Valid [EST]", Category = "Object"))
struct EXTENDEDSTATETREE_API FEstObjectIsValidPropertyFunction : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FEstObjectFunctionInstanceData;

	virtual UStruct const* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual void Execute(FStateTreeExecutionContext& Context) const override;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bInvert = false;

#if WITH_EDITOR
	virtual FText GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const override;
#endif
};