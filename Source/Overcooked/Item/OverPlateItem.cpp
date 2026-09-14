#include "OverPlateItem.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "OverKitchenSettings.h"
#include "../Core/OCRecipeLibrary.h"
#include "../Character/APlayerCharacter.h"
#include "../Item/ItemHolderComponent.h"

// 접시 메시와 음식을 부착할 기준점을 생성합니다.
AOverPlateItem::AOverPlateItem()
{
	ConfigureAsPlate(nullptr);
	FoodPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FoodPoint"));
	FoodPoint->SetupAttachment(GetRootComponent());
	CompletedDishMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CompletedDishMesh"));
	CompletedDishMesh->SetupAttachment(FoodPoint);
	CompletedDishMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CompletedDishMesh->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShrimpNigiri(
		TEXT("/Game/External/Quaternius/SushiRestaurant/Food/Food_EbiNigiri.Food_EbiNigiri"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> OctopusNigiri(
		TEXT("/Game/External/Quaternius/SushiRestaurant/Food/Food_OctopusNigiri.Food_OctopusNigiri"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SalmonNigiri(
		TEXT("/Game/External/Quaternius/SushiRestaurant/Food/Food_SalmonNigiri.Food_SalmonNigiri"));
	ShrimpNigiriMesh = ShrimpNigiri.Object;
	OctopusNigiriMesh = OctopusNigiri.Object;
	SalmonNigiriMesh = SalmonNigiri.Object;
}

void AOverPlateItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOverPlateItem, DisplayedRecipe);
}

// 들고 있는 재료 한 개를 빈 접시에 부착하고 성공 여부를 반환합니다.
bool AOverPlateItem::AddFood(AOverPickupItem* Ingredient)
{
	if (!CanBePickedUp() || Foods.Num() >= MaximumFoodCount || !IsValid(Ingredient) || Ingredient->IsA<AOverPlateItem>())
	{
		return false;
	}

	FOCPreparedIngredient PreparedIngredient;
	if (!Ingredient->BuildPreparedIngredient(PreparedIngredient)
		|| (Ingredient->CanBePlacedOnCuttingTable() && !Ingredient->IsChopped()))
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
	const int32 FoodIndex = Foods.Num();
	const FVector FoodOffset = FoodIndex == 0
		? FVector(-12.0f, 0.0f, 0.0f)
		: FoodIndex == 1
			? FVector(12.0f, 0.0f, 0.0f)
			: FoodIndex == 2
				? FVector(0.0f, 12.0f, 4.0f)
				: FVector(0.0f, -12.0f, 4.0f);
	Ingredient->AddActorLocalOffset(FoodOffset);
	Foods.Add(Ingredient);
	RefreshCompletedDishVisual();
	return true;
}

void AOverPlateItem::RefreshCompletedDishVisual()
{
	FOCDishContents Dish;
	const EOCRecipeType Recipe = BuildDishContents(Dish)
		? UOCRecipeLibrary::FindMatchingRecipe(Dish)
		: EOCRecipeType::None;
	DisplayedRecipe = GetCompletedDishMesh(Recipe) ? Recipe : EOCRecipeType::None;
	OnRep_DisplayedRecipe();
	ForceNetUpdate();
}

void AOverPlateItem::OnRep_DisplayedRecipe()
{
	if (!CompletedDishMesh)
	{
		return;
	}

	UStaticMesh* DishMesh = GetCompletedDishMesh(DisplayedRecipe);
	CompletedDishMesh->SetStaticMesh(DishMesh);
	CompletedDishMesh->SetVisibility(DishMesh != nullptr, true);

	for (AOverPickupItem* Food : Foods)
	{
		if (IsValid(Food))
		{
			Food->SetActorHiddenInGame(DishMesh != nullptr);
		}
	}
}

UStaticMesh* AOverPlateItem::GetCompletedDishMesh(const EOCRecipeType Recipe) const
{
	switch (Recipe)
	{
	case EOCRecipeType::ShrimpSushi:
		return ShrimpNigiriMesh;
	case EOCRecipeType::OctopusSushi:
		return OctopusNigiriMesh;
	case EOCRecipeType::SalmonSushi:
		return SalmonNigiriMesh;
	default:
		return nullptr;
	}
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
	for (const AOverPickupItem* Food : Foods)
	{
		if (IsValid(Food))
		{
			Params.AddIgnoredActor(Food);
		}
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
	for (const AOverPickupItem* Food : Foods)
	{
		if (IsValid(Food))
		{
			return true;
		}
	}
	return false;
}

bool AOverPlateItem::BuildDishContents(FOCDishContents& OutDish) const
{
	OutDish.Ingredients.Reset();

	for (const AOverPickupItem* Food : Foods)
	{
		if (!IsValid(Food))
		{
			continue;
		}

		FOCPreparedIngredient PreparedIngredient;
		if (!Food->BuildPreparedIngredient(PreparedIngredient))
		{
			return false;
		}

		OutDish.Ingredients.Add(PreparedIngredient);
	}
	return !OutDish.Ingredients.IsEmpty();
}

// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
void AOverPlateItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 접시가 제거될 때 부착된 음식도 함께 제거합니다.
	for (AOverPickupItem* Food : Foods)
	{
		if (IsValid(Food))
		{
			Food->Destroy();
		}
	}
	Foods.Reset();
	// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
	Super::EndPlay(EndPlayReason);
}

void AOverPlateItem::Interact_Implementation(AAPlayerCharacter* Player)
{
	if (!HasAuthority() || !Player || Foods.Num() >= MaximumFoodCount)
	{
		return;
	}

	UItemHolderComponent* Holder =
		Player->FindComponentByClass<UItemHolderComponent>();

	if (!Holder)
	{
		return;
	}

	AOverPickupItem* Ingredient =
		Cast<AOverPickupItem>(Holder->GetHeldObject());

	if (!Ingredient)
	{
		return;
	}

	if (AddFood(Ingredient))
	{
		Holder->CompleteTransfer(Ingredient);
	}
}

void AOverPlateItem::BeginPlay()
{
    Super::BeginPlay();
    ReturnTransform = GetActorTransform();
}
