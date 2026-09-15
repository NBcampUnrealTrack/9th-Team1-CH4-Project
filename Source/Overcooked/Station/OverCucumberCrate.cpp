#include "OverCucumberCrate.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "../Item/OverCucumberItem.h"

AOverCucumberCrate::AOverCucumberCrate()
{
	IngredientClass = AOverCucumberItem::StaticClass();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/Overcooked/Kitchen/StaticMeshes/SM_IngredientCrate_Cucumber.SM_IngredientCrate_Cucumber"));
	CrateMesh->SetStaticMesh(Mesh.Object);
}
