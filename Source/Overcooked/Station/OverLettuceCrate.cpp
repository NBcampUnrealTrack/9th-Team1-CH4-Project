#include "OverLettuceCrate.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "../Item/OverLettuceItem.h"

AOverLettuceCrate::AOverLettuceCrate()
{
	IngredientClass = AOverLettuceItem::StaticClass();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/External/KayKit/RestaurantBits/Environment/crate_lettuce.crate_lettuce"));
	CrateMesh->SetStaticMesh(Mesh.Object);
}
