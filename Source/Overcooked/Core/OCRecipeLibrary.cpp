#include "OCRecipeLibrary.h"

namespace
{
	FOCIngredientRequirement MakeRequirement(
		const EOCIngredientType Ingredient,
		const EOCIngredientState State,
		const int32 Quantity = 1)
	{
		FOCIngredientRequirement Requirement;
		Requirement.Ingredient = Ingredient;
		Requirement.RequiredState = State;
		Requirement.Quantity = Quantity;
		return Requirement;
	}
}

FOCRecipeDefinition UOCRecipeLibrary::GetRecipeDefinition(const EOCRecipeType Recipe)
{
	FOCRecipeDefinition Definition;
	Definition.Recipe = Recipe;

	switch (Recipe)
	{
	case EOCRecipeType::LettuceSalad:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "LettuceSalad", "양상추 샐러드");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "LettuceSaladIngredients", "썬 양상추");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Lettuce, EOCIngredientState::Chopped)
		};
		Definition.BaseScore = 20;
		break;

	case EOCRecipeType::TomatoSalad:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "TomatoSalad", "토마토 샐러드");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "TomatoSaladIngredients", "썬 양상추 + 토마토");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Lettuce, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Tomato, EOCIngredientState::Chopped)
		};
		Definition.BaseScore = 30;
		break;

	case EOCRecipeType::CucumberTomatoCabbageSalad:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "CucumberTomatoCabbageSalad", "오이 샐러드");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "CucumberTomatoCabbageSaladIngredients", "썬 오이 + 토마토 + 양배추");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Cucumber, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Tomato, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Cabbage, EOCIngredientState::Chopped)
		};
		Definition.BaseScore = 40;
		break;

	default:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "None", "None");
		Definition.IngredientSummary = FText::GetEmpty();
		break;
	}

	return Definition;
}

TArray<FOCRecipeDefinition> UOCRecipeLibrary::GetAllRecipeDefinitions()
{
	return {
		GetRecipeDefinition(EOCRecipeType::LettuceSalad),
		GetRecipeDefinition(EOCRecipeType::TomatoSalad),
		GetRecipeDefinition(EOCRecipeType::CucumberTomatoCabbageSalad)
	};
}

bool UOCRecipeLibrary::DoesDishMatchRecipe(const FOCDishContents& Dish, const EOCRecipeType Recipe)
{
	const FOCRecipeDefinition Definition = GetRecipeDefinition(Recipe);
	int32 RequiredIngredientCount = 0;

	//재료 종류 판정
	for (const FOCIngredientRequirement& Requirement : Definition.Requirements)
	{
		RequiredIngredientCount += Requirement.Quantity;
		int32 MatchingCount = 0;

		for (const FOCPreparedIngredient& Ingredient : Dish.Ingredients)
		{
			//손질 상태 판정
			if (Ingredient.Ingredient == Requirement.Ingredient
				&& Ingredient.State == Requirement.RequiredState)
			{
				++MatchingCount;
			}
		}

		if (MatchingCount != Requirement.Quantity)
		{
			return false;
		}
	}

	//재료 개수 판정
	return Definition.Recipe != EOCRecipeType::None
		&& Dish.Ingredients.Num() == RequiredIngredientCount;
}

//레시피 검색
EOCRecipeType UOCRecipeLibrary::FindMatchingRecipe(const FOCDishContents& Dish)
{
	for (const FOCRecipeDefinition& Definition : GetAllRecipeDefinitions())
	{
		if (DoesDishMatchRecipe(Dish, Definition.Recipe))
		{
			return Definition.Recipe;
		}
	}

	return EOCRecipeType::None;
}
