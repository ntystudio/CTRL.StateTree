#include "UstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UstUtils)

FStateTreePropertyPath UUstUtils::GetStructPropertyPath(FGuid const& ID, FName const A, FName const B)
{
	auto Path = FStateTreePropertyPath(ID);
	Path.FromString(FString::Printf(TEXT("%s.%s"), *A.ToString(), *B.ToString()));
	return Path;
}
