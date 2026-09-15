#include "OverOctopusCrate.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "../Item/OverOctopusItem.h"

AOverOctopusCrate::AOverOctopusCrate()
{
	IngredientClass = AOverOctopusItem::StaticClass();
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(
		TEXT("/Game/Overcooked/Kitchen/StaticMeshes/SM_IngredientFridge_Tentacle.SM_IngredientFridge_Tentacle"));
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
