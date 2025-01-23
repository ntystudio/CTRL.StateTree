#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"

#include "Components/StateTreeComponent.h"

#include "ExtendedStateTree/ExtendedStateTree.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "EstUtils.generated.h"

#define EST_GET_BINDING_TEXT(ID, InstanceDataView, BindingLookup, Formatting, MemberName, DefaultString) \
([&]() -> FText { \
	auto const Path = FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, MemberName)); \
	auto const Source = BindingLookup.GetPropertyBindingSource(Path); \
	FInstanceDataType const* Data = InstanceDataView.GetPtr<FInstanceDataType>(); \
	return Source ? FText::FromString(FString::Printf(TEXT("<s>{</s>%s<s>}</s>"), *BindingLookup.GetBindingSourceDisplayName(Path).ToString())) : FText::FromString(DefaultString); \
}())

UCLASS(DisplayName = "StateTree Utils [EST]", ClassGroup=(EST))
class EXTENDEDSTATETREE_API UEstUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	inline static FString SymbolStateEnter = TEXT("<s>↳</s>");
	inline static FString SymbolStateExit = TEXT("<s>↰</s>");
	inline static FString SymbolStateComplete = TEXT("<s>✓</s>");
	inline static FString SymbolTaskContinuous = TEXT("∞");
	inline static FString SymbolTaskEndStateOnError = TEXT("✗");
	inline static FString SymbolTaskEndStateOnDone = TEXT("✓");
	inline static FString SymbolInvalid = TEXT("🚩");

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateTree")
	static FString GetEndStateSymbol(bool const bEndsState) { return bEndsState ? SymbolTaskEndStateOnDone : SymbolTaskContinuous; }

	static FString ReplaceRichText(FString const& String, EStateTreeNodeFormatting const Formatting)
	{
		auto const BaseString = String
		                        .Replace(TEXT("STT_"), TEXT(""))
		                        .Replace(TEXT("STT"), TEXT(""));
		if (Formatting == RichText)
		{
			// fix up rich text, named closing tags are not supported
			return BaseString
			       .Replace(TEXT("</b>"), TEXT("</>"))
			       .Replace(TEXT("</s>"), TEXT("</>"));
		}

		if (!BaseString.Contains(TEXT("</s>")) && !BaseString.Contains(TEXT("</b>")))
		{
			// probably already formatted
			return BaseString
			       .Replace(TEXT("<s>"), TEXT(""))
			       .Replace(TEXT("<b>"), TEXT(""))
			       .Replace(TEXT("</>"), TEXT(""));
		}
		return BaseString
		       .Replace(TEXT("<s>"), TEXT(""))
		       .Replace(TEXT("<b>"), TEXT("\""))
		       .Replace(TEXT("</s>"), TEXT(""))
		       .Replace(TEXT("</b>"), TEXT("\""));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateTree")
	static FText FormatDescription(FString const& Description, EStateTreeNodeFormatting const Formatting = Text)
	{
		return FText::AsCultureInvariant(ReplaceRichText(Description, Formatting));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateTree")
	static FText FormatDescriptionText(FText const& Description, EStateTreeNodeFormatting const Formatting = Text)
	{
		return FText::AsCultureInvariant(ReplaceRichText(Description.ToString(), Formatting));
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateTree")
	static bool GetStateName(UStateTree const* Tree, FStateTreeStateHandle const& StateHandle, FName& OutName)
	{
		OutName = NAME_None;
		if (!StateHandle.IsValid())
		{
			EST_LOG(Warning, TEXT("StateTreeUtils::GetStateName: StateHandle is not valid."));
			return false;
		}
		if (!IsValid(Tree))
		{
			EST_LOG(Warning, TEXT("StateTreeUtils::GetStateName: Tree is not valid."));
			return false;
		}
		auto const State = Tree->GetStateFromHandle(StateHandle);
		if (!State)
		{
			EST_LOG(Warning, TEXT("StateTreeUtils::GetStateName: Could not find state from handle"));
			return false;
		}
		OutName = State->Name;
		return true;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateTree")
	static FGameplayTag GetStateTag(UStateTree const* Tree, FStateTreeStateHandle const& StateHandle)
	{
		if (!StateHandle.IsValid()) { return FGameplayTag::EmptyTag; }
		if (!IsValid(Tree)) { return FGameplayTag::EmptyTag; }
		auto const State = Tree->GetStateFromHandle(StateHandle);
		if (!State) { return FGameplayTag::EmptyTag; }
		return State->Tag;
	}

	static FStateTreePropertyPath GetStructPropertyPath(FGuid const& ID, FName A, FName B);
};
