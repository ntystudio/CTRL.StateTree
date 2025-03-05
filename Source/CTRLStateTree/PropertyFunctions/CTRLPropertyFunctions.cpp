// SPDX-FileCopyrightText: © 2025 NTY.studio
// SPDX-License-Identifier: MIT

#include "CTRLStateTree/PropertyFunctions/CTRLPropertyFunctions.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeNodeDescriptionHelpers.h"

#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CTRLPropertyFunctions)

#define LOCTEXT_NAMESPACE "CTRLStateTree"

//~ ━━━━━━━━━━━━━━━━━━━━━━━━ FCTRLRandomFloatPropertyFunction ━━━━━━━━━━━━━━━━━━━━━━━━━ //

void FCTRLRandomFloatPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Left, Right, Result] = Context.GetInstanceData(*this);
	Result = FMath::RandRange(Left, Right);
}

#if WITH_EDITOR
FText FCTRLRandomFloatPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	return UE::StateTree::DescHelpers::GetDescriptionForMathOperation<FInstanceDataType>(LOCTEXT("FloatRandom", "Random Float"), ID, InstanceDataView, BindingLookup, Formatting);
}
#endif

//~ ━━━━━━━━━━━━━━━━━━━━━━━━━ FCTRLRandomIntPropertyFunction ━━━━━━━━━━━━━━━━━━━━━━━━━━ //

void FCTRLRandomIntPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Left, Right, Result] = Context.GetInstanceData(*this);
	Result = FMath::RandRange(Left, Right);
}

#if WITH_EDITOR
FText FCTRLRandomIntPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	return UE::StateTree::DescHelpers::GetDescriptionForMathOperation<FInstanceDataType>(LOCTEXT("IntRandom", "Random Int"), ID, InstanceDataView, BindingLookup, Formatting);
}
#endif

//~ ━━━━━━━━━━━━━━━━━━━━━━━━ FCTRLStringToTextPropertyFunction ━━━━━━━━━━━━━━━━━━━━━━━━ //

void FCTRLStringToTextPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Input, Result] = Context.GetInstanceData(*this);
	Result = FText::FromString(Input);
}

#if WITH_EDITOR
FText FCTRLStringToTextPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const& [Input, Result] = InstanceDataView.Get<FInstanceDataType>();

	FText InputValue = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Input)), Formatting);
	if (InputValue.IsEmpty())
	{
		InputValue = FText::FromString(Input);
	}

	return UE::StateTree::DescHelpers::GetSingleParamFunctionText(LOCTEXT("StringToText", "String To Text"), InputValue, Formatting);
}
#endif

void FCTRLStringToNamePropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Input, Result] = Context.GetInstanceData(*this);
	Result = FName(Input);
}

FText FCTRLStringToNamePropertyFunction::GetDescription(FGuid const& ID, FStateTreeDataView InstanceDataView, IStateTreeBindingLookup const& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	auto const& [Input, Result] = InstanceDataView.Get<FInstanceDataType>();

	FText InputValue = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Input)), Formatting);
	if (InputValue.IsEmpty())
	{
		InputValue = FText::FromString(Input);
	}

	return UE::StateTree::DescHelpers::GetSingleParamFunctionText(LOCTEXT("StringToName", "String To Text"), InputValue, Formatting);
}

//~ ━━━━━━━━━━━━━━━━━━━━━━━━ FCTRLBoolToStringPropertyFunction ━━━━━━━━━━━━━━━━━━━━━━━━ //

void FCTRLBoolToStringPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Input, Result] = Context.GetInstanceData(*this);
	Result = LexToString(Input);
}

#if WITH_EDITOR
FText FCTRLBoolToStringPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const& [Input, Result] = InstanceDataView.Get<FInstanceDataType>();

	FText InputValue = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Input)), Formatting);
	if (InputValue.IsEmpty())
	{
		InputValue = FText::FromString(LexToString(Input));
	}

	return UE::StateTree::DescHelpers::GetSingleParamFunctionText(LOCTEXT("BoolToString", "Bool To String"), InputValue, Formatting);
}
#endif

//~ ━━━━━━━━━━━━━━━━━━━━━━━ FCTRLObjectIsValidPropertyFunction ━━━━━━━━━━━━━━━━━━━━━━━━ //

void FCTRLObjectIsValidPropertyFunction::Execute(FStateTreeExecutionContext& Context) const
{
	auto& [Left, Result, bInvert] = Context.GetInstanceData(*this);
	Result = IsValid(Left) != bInvert;
}

#if WITH_EDITOR
FText FCTRLObjectIsValidPropertyFunction::GetDescription(
	FGuid const& ID,
	FStateTreeDataView const InstanceDataView,
	IStateTreeBindingLookup const& BindingLookup,
	EStateTreeNodeFormatting const Formatting
) const
{
	auto const Data = InstanceDataView.Get<FInstanceDataType>();
	auto const InvertText = UE::StateTree::DescHelpers::GetBoolText(Data.bInvert, Formatting);
	auto const DescText = UE::StateTree::DescHelpers::GetDescriptionForSingleParameterFunc<FInstanceDataType>(
		LOCTEXT("ObjectIsValid", "Object Is Valid"),
		ID,
		InstanceDataView,
		BindingLookup,
		Formatting
	);
	return FText::Format(LOCTEXT("ObjectIsValidDesc", "{Invert}{Desc}"), InvertText, DescText);
}
#endif

#undef LOCTEXT_NAMESPACE
