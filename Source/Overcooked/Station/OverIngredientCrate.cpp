#include "OverIngredientCrate.h"

#include "Components/StaticMeshComponent.h"
#include "../Character/APlayerCharacter.h"
#include "../Item/ItemHolderComponent.h"
#include "../Item/OverPickupItem.h"

AOverIngredientCrate::AOverIngredientCrate()
{
	CrateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrateMesh"));
	SetRootComponent(CrateMesh);
	CrateMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void AOverIngredientCrate::Interact_Implementation(AAPlayerCharacter* Player)
{
	//재료 상자
	if (!HasAuthority() || !Player || !IngredientClass)
	{
		return;
	}

	UItemHolderComponent* Holder = Player->FindComponentByClass<UItemHolderComponent>();
	if (!Holder || Holder->GetHeldObject())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Player;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AOverPickupItem* Ingredient = GetWorld()->SpawnActor<AOverPickupItem>(
		IngredientClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParameters);
	if (!Ingredient)
	{
		return;
	}

	Holder->Hold(Ingredient);
	if (Holder->GetHeldObject() != Ingredient)
	{
		Ingredient->Destroy();
	}
}
