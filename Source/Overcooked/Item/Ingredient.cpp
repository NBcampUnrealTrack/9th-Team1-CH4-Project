#include "Ingredient.h"
#include "../Character/APlayerCharacter.h"
#include "ItemHolderComponent.h"

AIngredient::AIngredient()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AIngredient::Interact_Implementation(
	AAPlayerCharacter* Player
)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Ingredient 일반 상호작용")
	);
}

void AIngredient::Pickup_Implementation(
	AAPlayerCharacter* Player
)
{
	if (!Player)
	{
		return;
	}

	UItemHolderComponent* ItemHolder =
		Player->FindComponentByClass<UItemHolderComponent>();

	if (!ItemHolder)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Ingredient: ItemHolderComponent를 찾지 못함")
		);

		return;
	}

	ItemHolder->Hold(this);
}