#include "OverRiceCrate.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "../Item/OverRiceItem.h"

AOverRiceCrate::AOverRiceCrate()
{
	IngredientClass = AOverRiceItem::StaticClass();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/Overcooked/Kitchen/StaticMeshes/SM_IngredientContainer_Rice.SM_IngredientContainer_Rice"));
	CrateMesh->SetStaticMesh(Mesh.Object);

	BlockingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox"));
	BlockingBox->SetupAttachment(CrateMesh);
	BlockingBox->SetCollisionProfileName(TEXT("BlockAll"));
	if (Mesh.Object)
	{
		const FBoxSphereBounds Bounds = Mesh.Object->GetBounds();
		BlockingBox->SetRelativeLocation(Bounds.Origin);
		BlockingBox->SetBoxExtent(Bounds.BoxExtent.ComponentMax(FVector(1.0f)));
	}
}
