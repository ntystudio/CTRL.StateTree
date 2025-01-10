// SPDX-FileCopyrightText: 2025 NTY.studio

#include "ExtendedStateTree/PropertyFunctions/EstPropertyFunctions.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#define LOCTEXT_NAMESPACE "ExtendedStateTree"

void FEstRandomFloatPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Left, Right, Result] = Context.GetInstanceData(*this);
	Result = FMath::RandRange(Left, Right);
}

#if WITH_EDITOR
FText FEstRandomFloatPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	return UE::StateTree::DescHelpers::GetDescriptionForMathOperation<FInstanceDataType>(LOCTEXT("FloatRandom", "Random Float"), ID, InstanceDataView, BindingLookup, Formatting);
}

void FEstRandomIntPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	auto& [Left, Right, Result] = InstanceData;
	InstanceData.Result = FMath::RandRange(Left, Right);
}

FText FEstRandomIntPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	return UE::StateTree::DescHelpers::GetDescriptionForMathOperation<FInstanceDataType>(LOCTEXT("IntRandom", "Random Int"), ID, InstanceDataView, BindingLookup, Formatting);
}

void FEstStringToTextPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	auto& [Input, Result] = InstanceData;
	Result = FText::FromString(Input);
}

FText FEstStringToTextPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	FInstanceDataType const& InstanceData = InstanceDataView.Get<FInstanceDataType>();

	FText InputValue = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Input)), Formatting);
	if (InputValue.IsEmpty())
	{
		InputValue = FText::FromString(InstanceData.Input);
	}

	return UE::StateTree::DescHelpers::GetSingleParamFunctionText(LOCTEXT("StringToText", "String To Text"), InputValue, Formatting);
}

void FEstBoolToStringPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	auto& [Input, Result] = InstanceData;
	Result = LexToString(Input);
}

FText FEstBoolToStringPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	FInstanceDataType const& InstanceData = InstanceDataView.Get<FInstanceDataType>();

	FText InputValue = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Input)), Formatting);
	if (InputValue.IsEmpty())
	{
		InputValue = FText::FromString(LexToString(InstanceData.Input));
	}

	return UE::StateTree::DescHelpers::GetSingleParamFunctionText(LOCTEXT("BoolToString", "Bool To String"), InputValue, Formatting);
}

void FEstObjectIsValidPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	auto& [Left, Result] = InstanceData;
	InstanceData.Result = IsValid(Left) != bInvert;
}

FText FEstObjectIsValidPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const InvertText = UE::StateTree::DescHelpers::GetInvertText(bInvert, Formatting);
	auto const DescText = UE::StateTree::DescHelpers::GetDescriptionForSingleParameterFunc<FInstanceDataType>(
		LOCTEXT("ObjectIsValid", "Object Is Valid"),
		ID,
		InstanceDataView,
		BindingLookup,
		Formatting
	);
	return FText::Format(LOCTEXT("ObjectIsValidDesc", "{Invert}{Desc}"), InvertText, DescText);
}

#endif // WITH_EDITOR
#undef LOCTEXT_NAMESPACE

