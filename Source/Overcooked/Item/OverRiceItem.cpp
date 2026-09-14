#include "OverRiceItem.h"

AOverRiceItem::AOverRiceItem()
{
	RecipeIngredientType = EOCIngredientType::Rice;
	SetChoppingEnabled(false);
}

bool AOverRiceItem::BuildPreparedIngredient(FOCPreparedIngredient& OutIngredient) const
{
	OutIngredient.Ingredient = EOCIngredientType::Rice;
	OutIngredient.State = EOCIngredientState::Cooked;
	return true;
}
