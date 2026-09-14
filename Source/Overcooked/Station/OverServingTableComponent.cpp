#include "OverServingTableComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../Item/OverPickupItem.h"
#include "../Item/OverPlateItem.h"

#include "../APlayerCharacter.h"
#include "../ItemHolderComponent.h"
#include "../Core/OCGameMode.h"

// 타이머로 서빙을 처리하므로 매 프레임 갱신을 비활성화합니다.
UOverServingTableComponent::UOverServingTableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UOverServingTableComponent::TryServeHeldPlate(AAPlayerCharacter* Player)
{
	AActor* Table = GetOwner();

	if (!IsValid(Player)
		|| !IsValid(Table)
		|| !Table->HasAuthority()
		|| !bReady
		|| !FoodPoint)
	{
		return false;
	}

	UItemHolderComponent* Holder =
		Player->FindComponentByClass<UItemHolderComponent>();

	if (!Holder)
	{
		return false;
	}

	AOverPlateItem* HeldPlate =
		Cast<AOverPlateItem>(Holder->GetHeldObject());

	if (!IsValid(HeldPlate))
	{
		return false;
	}

	FOCDishContents Dish;

	if (!HeldPlate->BuildDishContents(Dish))
	{
		return false;
	}

	AOCGameMode* GameMode =
		GetWorld()->GetAuthGameMode<AOCGameMode>();

	if (!IsValid(GameMode))
	{
		return false;
	}

	EOCRecipeType MatchedRecipe =
		EOCRecipeType::None;

	// 현재 주문과 일치하는 음식만 서빙받습니다.
	if (!GameMode->SubmitDish(Dish, MatchedRecipe))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("서빙 실패: 현재 주문과 일치하지 않음")
		);

		return false;
	}

	if (!Serve(HeldPlate))
	{
		return false;
	}

	return Holder->CompleteTransfer(HeldPlate);
}

// 게임 시작 시 필요한 배치와 구성 요소를 준비합니다.
void UOverServingTableComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Table = GetOwner();

	if (!Table)
	{
		return;
	}

	UStaticMeshComponent* TableMesh =
		Table->FindComponentByClass<UStaticMeshComponent>();

	if (!TableMesh)
	{
		return;
	}

	const FBox Bounds = TableMesh->Bounds.GetBox();

	FoodPoint = NewObject<USceneComponent>(
		Table,
		TEXT("ServingFoodPoint")
	);

	if (!FoodPoint)
	{
		return;
	}

	// 서버와 클라이언트가 부착 대상을 같은 이름으로 식별하게 합니다.
	FoodPoint->SetNetAddressable();

	Table->AddInstanceComponent(FoodPoint);
	FoodPoint->SetupAttachment(Table->GetRootComponent());
	FoodPoint->RegisterComponent();

	FoodPoint->SetWorldLocation(
		FVector(
			Bounds.GetCenter().X,
			Bounds.GetCenter().Y,
			Bounds.Max.Z + SurfaceOffset
		)
	);

	bReady = true;
}

// 음식이 담긴 접시를 받아 반환 위치를 저장하고 정리 타이머를 시작합니다.
bool UOverServingTableComponent::Serve(AOverPickupItem* Ingredient)
{
	AOverPlateItem* ServedPlate = Cast<AOverPlateItem>(Ingredient);
	if (!bReady || !IsValid(ServedPlate) || !ServedPlate->HasFood()
		|| !ServedPlate->PlaceOn(FoodPoint, GetOwner()))
	{
		return false;
	}
	bReady = false;
	ServedIngredient = Ingredient;
	PlateReturnTransform = ServedPlate->ReturnTransform;
	ReturnedPlateClass = ServedPlate->GetClass();
	ReturnedPlateMesh = nullptr;
	ReturnedPlateMaterials.Reset();
	if (const UStaticMeshComponent* Mesh = ServedPlate->FindComponentByClass<UStaticMeshComponent>())
	{
		ReturnedPlateMesh = Mesh->GetStaticMesh();
		for (int32 Index = 0; Index < Mesh->GetNumMaterials(); ++Index)
		{
			ReturnedPlateMaterials.Add(Mesh->GetMaterial(Index));
		}
	}
	// PlaceOn에서 접시의 실제 밑면을 상판 높이에 맞춥니다.
	if (ClearDelay <= 0.0f)
	{
		ClearServing();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ServingTimer, this,
			&UOverServingTableComponent::ClearServing, ClearDelay, false);
	}
	return true;
}

// 상호작용 거리 판정에 사용할 배치 지점의 월드 위치를 반환합니다.
FVector UOverServingTableComponent::GetInteractionLocation() const
{
	return FoodPoint ? FoodPoint->GetComponentLocation() : GetOwner()->GetActorLocation();
}

// 서빙된 접시와 음식을 제거하고 빈 접시 반환 타이머를 시작합니다.
void UOverServingTableComponent::ClearServing()
{
	if (IsValid(ServedIngredient))
	{
		ServedIngredient->Destroy();
	}
	ServedIngredient = nullptr;
	if (PlateRespawnDelay <= 0.0f)
	{
		RestorePlate();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ServingTimer, this,
			&UOverServingTableComponent::RestorePlate, PlateRespawnDelay, false);
	}
}

// 저장된 원래 위치에 빈 접시 생성을 시도한 뒤 다음 서빙을 허용합니다.
void UOverServingTableComponent::RestorePlate()
{
	// 빈 접시는 서빙한 탁자가 아니라 원래 가져온 위치에 다시 생성합니다.
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AOverPlateItem* NewPlate = ReturnedPlateClass ? GetWorld()->SpawnActor<AOverPlateItem>(
		ReturnedPlateClass, PlateReturnTransform, Params) : nullptr;
	if (NewPlate)
	{
		NewPlate->ReturnTransform = PlateReturnTransform;
		if (UStaticMeshComponent* Mesh = NewPlate->FindComponentByClass<UStaticMeshComponent>())
		{
			Mesh->SetStaticMesh(ReturnedPlateMesh);
			for (int32 Index = 0; Index < ReturnedPlateMaterials.Num(); ++Index)
			{
				Mesh->SetMaterial(Index, ReturnedPlateMaterials[Index]);
			}
		}
	}
	ReturnedPlateClass = nullptr;
	ReturnedPlateMesh = nullptr;
	ReturnedPlateMaterials.Reset();
	bReady = true;
}

// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
void UOverServingTableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ServingTimer);
	if (IsValid(ServedIngredient))
	{
		ServedIngredient->Destroy();
	}
	// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
	Super::EndPlay(EndPlayReason);
}
