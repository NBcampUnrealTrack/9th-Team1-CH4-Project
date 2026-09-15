// 프로젝트 설정의 Description 페이지에서 저작권 문구를 설정할 수 있습니다.

#include "OverCuttingTable.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "../Item/OverKitchenSettings.h"
#include "../Item/OverPickupItem.h"
#include "../Character/APlayerCharacter.h"
#include "../Item/ItemHolderComponent.h"
#include "Net/UnrealNetwork.h"

// 작업대 메시와 재료 한 개를 올려놓을 부착 지점을 생성합니다.
AOverCuttingTable::AOverCuttingTable()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));
	SetRootComponent(TableMesh);
	TableMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	IngredientPoint = CreateDefaultSubobject<USceneComponent>(TEXT("IngredientPoint"));
	IngredientPoint->SetupAttachment(TableMesh);

	ChopTimeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ChopTimeText"));
	ChopTimeText->SetupAttachment(TableMesh);
	ChopTimeText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	ChopTimeText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	ChopTimeText->SetWorldSize(40.0f);
	ChopTimeText->SetTextRenderColor(FColor::Yellow);
	ChopTimeText->SetRelativeRotation(FRotator(60.0f, 0.0f, 0.0f));
	ChopTimeText->SetVisibility(false, true);

}

// 에디터에서 배치하거나 속성을 변경할 때 배치 설정을 갱신합니다.
void AOverCuttingTable::OnConstruction(const FTransform& Transform)
{
	// 에디터에서 배치하거나 속성을 변경할 때 배치 설정을 갱신합니다.
	Super::OnConstruction(Transform);
	if (!TableMesh->GetStaticMesh())
	{
		TableMesh->SetStaticMesh(GetDefault<UOverKitchenSettings>()->CuttingTableMesh.LoadSynchronous());
	}

	FVector LocalMin;
	FVector LocalMax;
	TableMesh->GetLocalBounds(LocalMin, LocalMax);
	const FVector TopCenter(
		(LocalMin.X + LocalMax.X) * 0.5f,
		(LocalMin.Y + LocalMax.Y) * 0.5f,
		LocalMax.Z);
	IngredientPoint->SetRelativeLocation(TopCenter + IngredientPointOffset);
	ChopTimeText->SetRelativeLocation(TopCenter + IngredientPointOffset + FVector(0.0f, 0.0f, 80.0f));
}

void AOverCuttingTable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOverCuttingTable, ReplicatedRemainingChopSeconds);
	DOREPLIFETIME(AOverCuttingTable, bShowChopDebugTime);
}

void AOverCuttingTable::Interact_Implementation(AAPlayerCharacter* Player)
{
	if (!HasAuthority() || !Player)
	{
		return;
	}

	UItemHolderComponent* Holder =
		Player->FindComponentByClass<UItemHolderComponent>();

	if (!Holder)
	{
		return;
	}

	AKitchenObject* HeldObject = Holder->GetHeldObject();

	// 손에 재료가 있으면 즉시 도마 위에 배치합니다.
	if (HeldObject)
	{
		AOverPickupItem* HeldItem =
			Cast<AOverPickupItem>(HeldObject);

		if (!HeldItem)
		{
			return;
		}

		if (PlaceIngredient(HeldItem))
		{
			Holder->CompleteTransfer(HeldItem);
		}

		return;
	}

	// 손이 비었고 썰린 재료가 있으면 다시 손으로 가져옵니다.
	if (IsValid(PlacedIngredient)
		&& PlacedIngredient->IsChopped())
	{
		AOverPickupItem* ChoppedIngredient =
			PlacedIngredient;

		Holder->Hold(ChoppedIngredient);

		if (Holder->GetHeldObject() == ChoppedIngredient)
		{
			PlacedIngredient = nullptr;
		}

		return;
	}

	// 손이 비었고 생재료가 있으면 썰기 타이머를 시작합니다.
	if (!CanChopIngredient()
		|| GetWorldTimerManager().IsTimerActive(ChoppingTimerHandle))
	{
		return;
	}

	ChoppingPlayer = Player;
	bShowChopDebugTime = true;
	UpdateChopDebugTime();

	const float SafeTickInterval =
		FMath::Max(ChopTickInterval, 0.01f);

	GetWorldTimerManager().SetTimer(
		ChoppingTimerHandle,
		this,
		&AOverCuttingTable::AdvanceChoppingTick,
		SafeTickInterval,
		true
	);

	Player->SetIsChopping(true);
}

void AOverCuttingTable::StopInteract_Implementation(
	AAPlayerCharacter* Player
)
{
	// F키를 뗐을 때는 중단하지 않습니다. 이동하거나 완료될 때 중단합니다.
}

void AOverCuttingTable::AdvanceChoppingTick()
{
	AAPlayerCharacter* Player = ChoppingPlayer.Get();

	if (!HasAuthority()
		|| !Player
		|| !CanChopIngredient())
	{
		PauseChopping();
		return;
	}

	if (Player->GetVelocity().SizeSquared2D() > FMath::Square(5.0f))
	{
		PauseChopping();
		return;
	}

	const float SafeTickInterval =
		FMath::Max(ChopTickInterval, 0.01f);

	if (AdvanceChopping(SafeTickInterval))
	{
		UpdateChopDebugTime();
		PauseChopping(true);
		return;
	}

	UpdateChopDebugTime();
}

void AOverCuttingTable::PauseChopping(const bool bHideDebugTime)
{
	GetWorldTimerManager().ClearTimer(ChoppingTimerHandle);

	if (AAPlayerCharacter* Player = ChoppingPlayer.Get())
	{
		Player->SetIsChopping(false);
	}

	ChoppingPlayer.Reset();

	if (bHideDebugTime)
	{
		bShowChopDebugTime = false;
	}

	OnRep_ChopDebugTime();
	ForceNetUpdate();
}

void AOverCuttingTable::UpdateChopDebugTime()
{
	ReplicatedRemainingChopSeconds = IsValid(PlacedIngredient)
		? PlacedIngredient->GetRemainingChopSeconds()
		: 0.0f;
	OnRep_ChopDebugTime();
	ForceNetUpdate();
}

void AOverCuttingTable::OnRep_ChopDebugTime()
{
	if (!ChopTimeText)
	{
		return;
	}

	ChopTimeText->SetText(FText::FromString(FString::Printf(
		TEXT("%.1f"),
		ReplicatedRemainingChopSeconds)));
	ChopTimeText->SetVisibility(bShowChopDebugTime, true);
}

// 작업대가 비어 있고 재료를 배치할 수 있을 때 부착 후 보관합니다.
bool AOverCuttingTable::PlaceIngredient(AOverPickupItem* Ingredient)
{
	if (!CanPlaceIngredient(Ingredient))
	{
		return false;
	}

	if (!Ingredient->PlaceOn(IngredientPoint, this))
	{
		return false;
	}

	PlacedIngredient = Ingredient;
	bShowChopDebugTime = false;
	UpdateChopDebugTime();
	return true;
}

// 작업대의 재료를 손으로 옮기며, 실패하거나 재료가 없으면 nullptr를 반환합니다.
AOverPickupItem* AOverCuttingTable::TakeIngredient(USceneComponent* HoldPoint, AActor* NewOwner)
{
	if (!IsValid(PlacedIngredient))
	{
		PlacedIngredient = nullptr;
		return nullptr;
	}

	AOverPickupItem* Ingredient = PlacedIngredient;
	if (!Ingredient->PickUp(HoldPoint, NewOwner))
	{
		return nullptr;
	}

	PlacedIngredient = nullptr;
	bShowChopDebugTime = false;
	UpdateChopDebugTime();
	return Ingredient;
}

// 작업대가 비어 있고 전달된 재료를 올릴 수 있는지 확인합니다.
bool AOverCuttingTable::CanPlaceIngredient(const AOverPickupItem* Ingredient) const
{
	return !IsValid(PlacedIngredient) && IsValid(Ingredient) && Ingredient->CanBePlacedOnCuttingTable();
}

// 작업대 위에 썰 수 있는 재료가 있는지 확인합니다.
bool AOverCuttingTable::CanChopIngredient() const
{
	return IsValid(PlacedIngredient) && PlacedIngredient->CanBeChopped();
}

// 썰기 시간을 누적하며, 이번 호출로 썰기가 완료되면 true를 반환합니다.
bool AOverCuttingTable::AdvanceChopping(float DeltaSeconds)
{
	return IsValid(PlacedIngredient) && PlacedIngredient->AdvanceChopping(DeltaSeconds);
}

// 상호작용 거리 판정에 사용할 배치 지점의 월드 위치를 반환합니다.
FVector AOverCuttingTable::GetInteractionLocation() const
{
	return IngredientPoint ? IngredientPoint->GetComponentLocation() : GetActorLocation();
}

void AOverCuttingTable::BeginPlay()
{
    Super::BeginPlay();
    OnConstruction(GetActorTransform());
}
