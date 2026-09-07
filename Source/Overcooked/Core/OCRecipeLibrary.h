#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OCRecipeTypes.h"
#include "OCRecipeLibrary.generated.h"

UCLASS()
class OVERCOOKED_API UOCRecipeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Overcooked|Recipe")
	static FOCRecipeDefinition GetRecipeDefinition(EOCRecipeType Recipe); //레시피.데이터

	UFUNCTION(BlueprintPure, Category = "Overcooked|Recipe")
	static TArray<FOCRecipeDefinition> GetAllRecipeDefinitions(); //레시피.데이터

	UFUNCTION(BlueprintPure, Category = "Overcooked|Recipe")
	static bool DoesDishMatchRecipe(const FOCDishContents& Dish, EOCRecipeType Recipe); //레시피.판정

	UFUNCTION(BlueprintPure, Category = "Overcooked|Recipe")
	static EOCRecipeType FindMatchingRecipe(const FOCDishContents& Dish); //레시피.판정
};
