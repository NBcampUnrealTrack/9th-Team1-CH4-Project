#pragma once

#include "CoreMinimal.h"
#include "OCMatchTypes.generated.h"

UENUM(BlueprintType)
enum class EOCMatchPhase : uint8
{
	Waiting		UMETA(DisplayName = "Waiting"),
	Countdown	UMETA(DisplayName = "Countdown"),
	Playing		UMETA(DisplayName = "Playing"),
	Ending		UMETA(DisplayName = "Ending"),
	Results		UMETA(DisplayName = "Results")
};

USTRUCT(BlueprintType)
struct OVERCOOKED_API FOCMatchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	int32 FinalScore = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	int32 EarnedStars = 0;
};
