#include "OverTomatoCrate.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "../Item/OverTomatoItem.h"

AOverTomatoCrate::AOverTomatoCrate()
{
	IngredientClass = AOverTomatoItem::StaticClass();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/External/KayKit/RestaurantBits/Environment/crate_tomatoes.crate_tomatoes"));
	CrateMesh->SetStaticMesh(Mesh.Object);
}
