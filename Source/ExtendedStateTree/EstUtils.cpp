#include "EstUtils.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EstUtils)

FStateTreePropertyPath UEstUtils::GetStructPropertyPath(FGuid const& ID, FName const A, FName const B)
{
	auto Path = FStateTreePropertyPath(ID);
	Path.FromString(FString::Printf(TEXT("%s.%s"), *A.ToString(), *B.ToString()));
	return Path;
}
