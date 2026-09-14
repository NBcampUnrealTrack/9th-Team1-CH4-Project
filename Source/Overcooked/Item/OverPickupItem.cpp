// 프로젝트 설정의 Description 페이지에서 저작권 문구를 설정할 수 있습니다.

#include "OverPickupItem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "OverKitchenSettings.h"
#include "../Character/APlayerCharacter.h"
#include "../Item/ItemHolderComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"


// 물리 충돌이 가능한 아이템 메시를 만들고 기본 재료를 설정합니다.
AOverPickupItem::AOverPickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	ItemMesh->SetSimulatePhysics(true);
}

// 에디터에서 배치하거나 속성을 변경할 때 배치 설정을 갱신합니다.
void AOverPickupItem::OnConstruction(const FTransform& Transform)
{
	// 에디터에서 배치하거나 속성을 변경할 때 배치 설정을 갱신합니다.
	Super::OnConstruction(Transform);

	if (!ItemMesh->GetStaticMesh())
	{
		ItemMesh->SetStaticMesh(GetDefault<UOverKitchenSettings>()->FindItemMesh(GetClass()));
	}
	if (bAutoSelectChoppedMesh && IngredientState == EOverIngredientState::Raw)
	{
		ChoppedMesh = FindAutomaticChoppedMesh();
	}
}

void AOverPickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOverPickupItem, IngredientState);
	DOREPLIFETIME(AOverPickupItem, bIsHeld);
	DOREPLIFETIME(AOverPickupItem, bIsPlaced);
	DOREPLIFETIME(AOverPickupItem, bIsOnTable);
}

void AOverPickupItem::OnRep_Owner()
{
	Super::OnRep_Owner();

	// 상태보다 Owner가 늦게 도착한 경우 다시 손에 부착합니다.
	if (bIsHeld)
	{
		ApplyReplicatedTransportState();
	}
}

void AOverPickupItem::OnRep_TransportState()
{
	ApplyReplicatedTransportState();
}

void AOverPickupItem::OnRep_IngredientState()
{
	if (!ItemMesh
		|| IngredientState != EOverIngredientState::Chopped)
	{
		return;
	}

	if (!ChoppedMesh && bAutoSelectChoppedMesh)
	{
		ChoppedMesh = FindAutomaticChoppedMesh();
	}

	if (ChoppedMesh)
	{
		ItemMesh->SetStaticMesh(ChoppedMesh);
	}
}

void AOverPickupItem::ApplyReplicatedTransportState()
{
	if (!ItemMesh)
	{
		return;
	}

	if (bIsHeld)
	{
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);
		SetActorEnableCollision(false);

		if (AAPlayerCharacter* HoldingPlayer =
			Cast<AAPlayerCharacter>(GetOwner()))
		{
			if (USkeletalMeshComponent* PlayerMesh =
				HoldingPlayer->GetMesh())
			{
				AttachToComponent(
					PlayerMesh,
					FAttachmentTransformRules::
					SnapToTargetNotIncludingScale,
					TEXT("HoldSocket")
				);

				SetActorRelativeLocation(
					HeldTransform.GetLocation()
				);

				SetActorRelativeRotation(
					HeldTransform.Rotator()
				);
			}
		}

		return;
	}

	if (bIsPlaced)
	{
		ItemMesh->SetSimulatePhysics(false);
		SetActorEnableCollision(false);
		return;
	}

	if (bIsOnTable)
	{
		// 탁자 위에서는 물리로 떨어지지 않게 고정합니다.
		ItemMesh->SetSimulatePhysics(false);

		// 서버의 E키 검색에 잡히도록 조회 충돌만 켭니다.
		SetActorEnableCollision(true);
		ItemMesh->SetCollisionEnabled(
			ECollisionEnabled::QueryOnly
		);

		return;
	}

	// 아무 곳에도 고정되지 않았다면 바닥에 떨어진 상태입니다.
	DetachFromActor(
		FDetachmentTransformRules::KeepWorldTransform
	);

	SetActorEnableCollision(true);

	ItemMesh->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics
	);

	ItemMesh->SetSimulatePhysics(true);
}

// 손 부착 지점이 유효하고 들고 있지 않으면 부착하며, 성공 여부를 반환합니다.
bool AOverPickupItem::PickUp(USceneComponent* HoldPoint, AActor* NewOwner, FName SocketName)
{
	if (!HoldPoint || bIsHeld)
	{
		return false;
	}

	// 들고 있는 동안 물리와 충돌을 끄고 캐릭터의 손 위치에 부착합니다.
	ItemMesh->SetSimulatePhysics(false);
	SetActorEnableCollision(false);
	SetOwner(NewOwner);
	AttachToComponent(HoldPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	SetActorRelativeLocation(HeldTransform.GetLocation());
	SetActorRelativeRotation(HeldTransform.Rotator());
	bIsHeld = true;
	bIsPlaced = false;
	bIsOnTable = false;
	ForceNetUpdate();


	return true;
}

// 손에 든 아이템을 지정 지점에 고정하고 배치 성공 여부를 반환합니다.
bool AOverPickupItem::PlaceOn(USceneComponent* PlacementPoint, AActor* NewOwner)
{
	if (!PlacementPoint || !bIsHeld)
	{
		return false;
	}

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ItemMesh->SetSimulatePhysics(false);
	SetActorEnableCollision(false);
	SetOwner(NewOwner);
	AttachToComponent(PlacementPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorRelativeLocation(FVector::ZeroVector);
	SetActorRelativeRotation(FRotator::ZeroRotator);
	// 메시 원점이 아닌 실제 밑면과 수평 중심을 배치 지점에 맞춥니다.
	AlignBottomTo(PlacementPoint->GetComponentLocation());
	bIsHeld = false;
	bIsPlaced = true;
	bIsOnTable = false;
	ForceNetUpdate();

	return true;
}

// 월드 경계 상자의 밑면 중앙을 목표 표면 위치에 맞춥니다.
void AOverPickupItem::AlignBottomTo(const FVector& SurfaceCenter)
{
	const FBox Bounds = ItemMesh->Bounds.GetBox();
	const FVector BottomCenter(Bounds.GetCenter().X, Bounds.GetCenter().Y, Bounds.Min.Z);
	AddActorWorldOffset(SurfaceCenter - BottomCenter);
}

// 일반 탁자 윗면에 배치하고 다시 집을 수 있도록 조회 충돌을 켭니다.
bool AOverPickupItem::PlaceOnTable(UStaticMeshComponent* TableMesh)
{
	if (!IsValid(TableMesh)
		|| !PlaceOn(TableMesh, TableMesh->GetOwner()))
	{
		return false;
	}

	const FBox TableBounds =
		TableMesh->Bounds.GetBox();

	const FVector TableSurfaceCenter(
		TableBounds.GetCenter().X,
		TableBounds.GetCenter().Y,
		TableBounds.Max.Z + TableSurfaceOffset
	);

	// 아이템의 밑면을 탁자 윗면 중앙에 맞춥니다.
	AlignBottomTo(TableSurfaceCenter);

	// 전용 시설 배치가 아니라 일반 탁자 배치 상태로 변경합니다.
	bIsPlaced = false;
	bIsOnTable = true;

	// 물리는 끄되 E키 픽업 검사는 가능하게 합니다.
	ItemMesh->SetSimulatePhysics(false);
	SetActorEnableCollision(true);

	ItemMesh->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	ForceNetUpdate();

	return true;
}

// 손에 든 아이템을 지정 위치와 회전으로 내려놓고 물리를 다시 켭니다.
void AOverPickupItem::Drop(const FVector& DropLocation, const FRotator& DropRotation)
{
	if (!bIsHeld)
	{
		return;
	}

	// 손에서 분리한 뒤 캐릭터 앞쪽에 놓고 다시 물리와 충돌을 활성화합니다.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorLocationAndRotation(DropLocation, DropRotation, false, nullptr, ETeleportType::TeleportPhysics);
	SetOwner(nullptr);
	SetActorEnableCollision(true);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetSimulatePhysics(true);
	bIsHeld = false;
	bIsPlaced = false;
	bIsOnTable = false;

	ForceNetUpdate();
}

// 손에 들려 있거나 전용 배치 상태인 아이템은 일반 줍기 대상에서 제외합니다.
bool AOverPickupItem::CanBePickedUp() const
{
	return !bIsHeld && !bIsPlaced;
}

// 현재 손에 들고 있는 썰기 지원 아이템인지 확인합니다.
bool AOverPickupItem::CanBePlacedOnCuttingTable() const
{
	return bIsHeld && bCanBeChopped;
}

// 배치된 재료가 썰기 가능한 원재료 상태인지 확인합니다.
bool AOverPickupItem::CanBeChopped() const
{
	return bIsPlaced && bCanBeChopped && ChoppedMesh && IngredientState == EOverIngredientState::Raw;
}

// 썰기 시간을 누적하며, 이번 호출로 썰기가 완료되면 true를 반환합니다.
bool AOverPickupItem::AdvanceChopping(float DeltaSeconds)
{
	if (!CanBeChopped())
	{
		return false;
	}

	ChopProgressSeconds += FMath::Max(DeltaSeconds, 0.0f);
	if (ChopProgressSeconds < FMath::Max(RequiredChopSeconds, 0.0f))
	{
		return false;
	}

	IngredientState = EOverIngredientState::Chopped;
	if (ChoppedMesh)
	{
		const FBox PreviousBounds = ItemMesh->Bounds.GetBox();
		const FVector SurfaceCenter(PreviousBounds.GetCenter().X, PreviousBounds.GetCenter().Y, PreviousBounds.Min.Z);
		ItemMesh->SetStaticMesh(ChoppedMesh);
		// 썰린 메시의 원점과 크기가 달라져도 도마 위 위치를 유지합니다.
		AlignBottomTo(SurfaceCenter);
	}

	ForceNetUpdate();

	return true;
}

// 재료가 썰기 완료 상태인지 반환합니다.
bool AOverPickupItem::IsChopped() const
{
	return IngredientState == EOverIngredientState::Chopped;
}

bool AOverPickupItem::BuildPreparedIngredient(FOCPreparedIngredient& OutIngredient) const
{
	if (RecipeIngredientType == EOCIngredientType::None)
	{
		return false;
	}

	OutIngredient.Ingredient = RecipeIngredientType;
	OutIngredient.State = IsChopped() ? EOCIngredientState::Chopped : EOCIngredientState::Whole;

	return true;
}

void AOverPickupItem::Pickup_Implementation(AAPlayerCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	if (UItemHolderComponent* ItemHolder =
		Player->FindComponentByClass<UItemHolderComponent>())
	{
		ItemHolder->Hold(this);
	}
}

// 접시 메시를 지정하고 썰기 기능과 물리 시뮬레이션을 끕니다.
void AOverPickupItem::ConfigureAsPlate(UStaticMesh* PlateMesh)
{
	// 원본과 결과 메시를 직접 지정하고 메시 자동 선택을 끕니다.
	SetIngredientMeshes(PlateMesh, nullptr);
	bCanBeChopped = false;
	ItemMesh->SetSimulatePhysics(false);
}

// 원본과 결과 메시를 직접 지정하고 메시 자동 선택을 끕니다.
void AOverPickupItem::SetIngredientMeshes(UStaticMesh* RawMesh, UStaticMesh* NewChoppedMesh)
{
	if (RawMesh)
	{
		ItemMesh->SetStaticMesh(RawMesh);
	}

	ChoppedMesh = NewChoppedMesh;
	bAutoSelectChoppedMesh = false;
}

// 원본 이름에 대응하는 결과 메시를 로드하며, 대응 항목이 없으면 nullptr를 반환합니다.
UStaticMesh* AOverPickupItem::FindAutomaticChoppedMesh() const
{
    return GetDefault<UOverKitchenSettings>()->FindChoppedMesh(ItemMesh ? ItemMesh->GetStaticMesh() : nullptr);
}

void AOverPickupItem::BeginPlay()
{
    Super::BeginPlay();
    if (!ItemMesh->GetStaticMesh())
    {
        ItemMesh->SetStaticMesh(GetDefault<UOverKitchenSettings>()->FindItemMesh(GetClass()));
    }
    if (bAutoSelectChoppedMesh && IngredientState == EOverIngredientState::Raw)
    {
        ChoppedMesh = FindAutomaticChoppedMesh();
    }
}

void AOverPickupItem::SetChoppingEnabled(const bool bEnabled)
{
	bCanBeChopped = bEnabled;
}
