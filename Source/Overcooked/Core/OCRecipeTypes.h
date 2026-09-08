#pragma once

#include "CoreMinimal.h"
#include "OCRecipeTypes.generated.h"

UENUM(BlueprintType)
enum class EOCRecipeType : uint8
{
	None,
	LettuceSalad,
	TomatoSalad,
	CucumberTomatoCabbageSalad
};

UENUM(BlueprintType)
enum class EOCIngredientType : uint8
{
	Lettuce,
	Tomato,
	Cucumber,
	Cabbage
};

UENUM(BlueprintType)
enum class EOCIngredientState : uint8
{
	Whole,
	Chopped,
	Cooked
};

USTRUCT(BlueprintType)
struct OVERCOOKED_API FOCPreparedIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	EOCIngredientType Ingredient = EOCIngredientType::Lettuce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	EOCIngredientState State = EOCIngredientState::Whole;
};

USTRUCT(BlueprintType)
struct OVERCOOKED_API FOCIngredientRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	EOCIngredientType Ingredient = EOCIngredientType::Lettuce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	EOCIngredientState RequiredState = EOCIngredientState::Whole;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "1"))
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct OVERCOOKED_API FOCRecipeDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	EOCRecipeType Recipe = EOCRecipeType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	FText IngredientSummary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	TArray<FOCIngredientRequirement> Requirements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "0"))
	int32 BaseScore = 0;
};

USTRUCT(BlueprintType)
struct OVERCOOKED_API FOCDishContents
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	TArray<FOCPreparedIngredient> Ingredients;
};

USTRUCT(BlueprintType)
struct OVERCOOKED_API FOCActiveOrder
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Order")
	int32 OrderId = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Order")
	EOCRecipeType Recipe = EOCRecipeType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Order")
	float CreatedAtServerTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Order")
	float TimeLimit = 30.0f;
};
