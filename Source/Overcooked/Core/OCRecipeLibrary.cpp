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
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "TomatoSaladIngredients", "썬 양상추 + 썬 토마토");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Lettuce, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Tomato, EOCIngredientState::Chopped)
		};
		Definition.BaseScore = 30;
		break;

	case EOCRecipeType::CucumberTomatoCabbageSalad:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "CucumberTomatoCabbageSalad", "오이 샐러드");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "CucumberTomatoCabbageSaladIngredients", "썬 오이 + 썬 토마토 + 썬 양배추");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Cucumber, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Tomato, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Cabbage, EOCIngredientState::Chopped)
		};
		Definition.BaseScore = 40;
		break;

	case EOCRecipeType::ShrimpSushi:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "ShrimpSushi", "새우 초밥");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "ShrimpSushiIngredients", "새우 + 밥");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Shrimp, EOCIngredientState::Whole),
			MakeRequirement(EOCIngredientType::Rice, EOCIngredientState::Cooked)
		};
		Definition.BaseScore = 40;
		break;

	case EOCRecipeType::OctopusSushi:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "OctopusSushi", "문어 초밥");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "OctopusSushiIngredients", "썬 문어 + 밥");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Octopus, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Rice, EOCIngredientState::Cooked)
		};
		Definition.BaseScore = 45;
		break;

	case EOCRecipeType::SalmonSushi:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "SalmonSushi", "연어 초밥");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "SalmonSushiIngredients", "연어 필렛 + 밥");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::SalmonFillet, EOCIngredientState::Whole),
			MakeRequirement(EOCIngredientType::Rice, EOCIngredientState::Cooked)
		};
		Definition.BaseScore = 40;
		break;

	case EOCRecipeType::TomatoBaconSalad:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "TomatoBaconSalad", "토마토 베이컨 샐러드");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "TomatoBaconSaladIngredients", "썬 양상추 + 썬 토마토 + 구운 베이컨");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::Lettuce, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Tomato, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Bacon, EOCIngredientState::Cooked)
		};
		Definition.BaseScore = 50;
		break;

	case EOCRecipeType::SalmonRollSushi:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "SalmonRollSushi", "연어 롤 초밥");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "SalmonRollSushiIngredients", "연어 필렛 + 밥 + 김");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::SalmonFillet, EOCIngredientState::Whole),
			MakeRequirement(EOCIngredientType::Rice, EOCIngredientState::Cooked),
			MakeRequirement(EOCIngredientType::Seaweed, EOCIngredientState::Whole)
		};
		Definition.BaseScore = 50;
		break;

	case EOCRecipeType::SeaUrchinRollSushi:
		Definition.DisplayName = NSLOCTEXT("OCRecipes", "SeaUrchinRollSushi", "성게알 롤 초밥");
		Definition.IngredientSummary = NSLOCTEXT("OCRecipes", "SeaUrchinRollSushiIngredients", "자른 성게 + 밥 + 김");
		Definition.Requirements = {
			MakeRequirement(EOCIngredientType::SeaUrchin, EOCIngredientState::Chopped),
			MakeRequirement(EOCIngredientType::Rice, EOCIngredientState::Cooked),
			MakeRequirement(EOCIngredientType::Seaweed, EOCIngredientState::Whole)
		};
		Definition.BaseScore = 55;
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
		GetRecipeDefinition(EOCRecipeType::CucumberTomatoCabbageSalad),
		GetRecipeDefinition(EOCRecipeType::ShrimpSushi),
		GetRecipeDefinition(EOCRecipeType::OctopusSushi),
		GetRecipeDefinition(EOCRecipeType::SalmonSushi),
		GetRecipeDefinition(EOCRecipeType::TomatoBaconSalad),
		GetRecipeDefinition(EOCRecipeType::SalmonRollSushi),
		GetRecipeDefinition(EOCRecipeType::SeaUrchinRollSushi)
	};
}

TArray<FOCRecipeDefinition> UOCRecipeLibrary::GetRecipeDefinitionsForStage(const EOCRecipeStage Stage)
{
	TArray<EOCRecipeType> RecipeTypes;
	switch (Stage)
	{
	case EOCRecipeStage::Tutorial:
		RecipeTypes = {
			EOCRecipeType::LettuceSalad,
			EOCRecipeType::TomatoSalad
		};
		break;

	case EOCRecipeStage::Stage1:
		RecipeTypes = {
			EOCRecipeType::LettuceSalad,
			EOCRecipeType::TomatoSalad,
			EOCRecipeType::CucumberTomatoCabbageSalad
		};
		break;

	case EOCRecipeStage::Stage2:
	case EOCRecipeStage::Stage3:
		RecipeTypes = {
			EOCRecipeType::ShrimpSushi,
			EOCRecipeType::OctopusSushi,
			EOCRecipeType::SalmonSushi
		};
		break;

	case EOCRecipeStage::PostStage3:
		RecipeTypes = {
			EOCRecipeType::TomatoBaconSalad,
			EOCRecipeType::SalmonRollSushi,
			EOCRecipeType::SeaUrchinRollSushi
		};
		break;
	}

	TArray<FOCRecipeDefinition> Definitions;
	Definitions.Reserve(RecipeTypes.Num());
	for (const EOCRecipeType RecipeType : RecipeTypes)
	{
		Definitions.Add(GetRecipeDefinition(RecipeType));
	}
	return Definitions;
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
