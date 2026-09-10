#include "OverPlateItem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "OverKitchenSettings.h"

// 접시 메시와 음식을 부착할 기준점을 생성합니다.
AOverPlateItem::AOverPlateItem()
{
	ConfigureAsPlate(nullptr);
	FoodPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FoodPoint"));
	FoodPoint->SetupAttachment(GetRootComponent());
}

// 들고 있는 재료 한 개를 빈 접시에 부착하고 성공 여부를 반환합니다.
bool AOverPlateItem::AddFood(AOverPickupItem* Ingredient)
{
	// 내려놓은 빈 접시에만 재료 한 개를 담습니다. 접시 위에 접시는 놓지 않습니다.
	if (!CanBePickedUp() || HasFood() || !IsValid(Ingredient) || Ingredient->IsA<AOverPlateItem>()
		|| !Ingredient->CanBePlacedOnCuttingTable())
	{
		return false;
	}
	if (UStaticMeshComponent* PlateMesh = FindComponentByClass<UStaticMeshComponent>())
	{
		// 접시의 가장 높은 테두리 대신 중앙의 실제 표면에 채소 밑면을 맞춥니다.
		const FBox Bounds = PlateMesh->Bounds.GetBox();
		const FVector Center = Bounds.GetCenter();
		FoodPoint->SetWorldLocation(Center);
		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(PlateFoodSurface), true);
		if (PlateMesh->LineTraceComponent(Hit,
			FVector(Center.X, Center.Y, Bounds.Max.Z + SurfaceTracePadding),
			FVector(Center.X, Center.Y, Bounds.Min.Z - SurfaceTracePadding), Params))
		{
			FoodPoint->SetWorldLocation(Hit.ImpactPoint);
		}
	}
	if (!Ingredient->PlaceOn(FoodPoint, this))
	{
		return false;
	}
	Food = Ingredient;
	return true;
}

// 탁자 위 접시는 캐릭터에 가까운 탁자 경계를 거리 판정 위치로 사용합니다.
FVector AOverPlateItem::GetPickupReachLocation(const FVector& CharacterLocation) const
{
	const UStaticMeshComponent* PlateMesh = FindComponentByClass<UStaticMeshComponent>();
	if (!PlateMesh)
	{
		return GetActorLocation();
	}
	const FBox PlateBounds = PlateMesh->Bounds.GetBox();
	const FVector PlateBottom(PlateBounds.GetCenter().X, PlateBounds.GetCenter().Y, PlateBounds.Min.Z);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PlateSupport), true, this);
	if (IsValid(Food))
	{
		Params.AddIgnoredActor(Food);
	}
	FHitResult Hit;
	// 접시 바로 아래의 탁자를 찾습니다. 바닥에 내려놓은 접시는 기존 거리 판정을 사용합니다.
	if (GetWorld()->LineTraceSingleByChannel(Hit, PlateBottom + FVector(0.0f, 0.0f, SupportTraceStartOffset),
		PlateBottom - FVector(0.0f, 0.0f, SupportTraceDepth), SupportTraceChannel, Params))
	{
		const UStaticMeshComponent* TableMesh = Cast<UStaticMeshComponent>(Hit.GetComponent());
		if (TableMesh && TableMesh->GetStaticMesh()
			&& GetDefault<UOverKitchenSettings>()->IsTableMesh(TableMesh->GetStaticMesh()))
		{
			const FBox TableBounds = TableMesh->Bounds.GetBox();
			// 탁자 중앙이 멀어도 가장자리에 접근하면 집을 수 있도록 수평 거리를 보정합니다.
			return FVector(FMath::Clamp(CharacterLocation.X, TableBounds.Min.X, TableBounds.Max.X),
				FMath::Clamp(CharacterLocation.Y, TableBounds.Min.Y, TableBounds.Max.Y), PlateBottom.Z);
		}
	}
	return PlateBottom;
}

// 접시에 담긴 음식 액터가 유효한지 확인합니다.
bool AOverPlateItem::HasFood() const
{
	return IsValid(Food);
}

// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
void AOverPlateItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 접시가 제거될 때 부착된 음식도 함께 제거합니다.
	if (IsValid(Food))
	{
		Food->Destroy();
	}
	// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
	Super::EndPlay(EndPlayReason);
}

void AOverPlateItem::BeginPlay()
{
    Super::BeginPlay();
    ReturnTransform = GetActorTransform();
}