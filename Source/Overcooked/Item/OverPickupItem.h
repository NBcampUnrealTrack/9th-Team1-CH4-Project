
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Item//KitchenObject.h"
#include "../Interaction/PickupableInterface.h"
#include "../Core/OCRecipeTypes.h"

#include "OverPickupItem.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class USceneComponent;

// 재료의 썰기 전후 상태를 블루프린트에서도 구분합니다.
UENUM(BlueprintType)
enum class EOverIngredientState : uint8
{
	Raw,
	Chopped
};

/** 캐릭터가 집어서 들거나 내려놓을 수 있는 채소·접시의 공통 클래스입니다. */
UCLASS(Blueprintable)
class OVERCOOKED_API AOverPickupItem : public AKitchenObject, public IPickupableInterface
{
	GENERATED_BODY()

public:
	// 물리 충돌이 가능한 아이템 메시를 만들고 기본 재료를 설정합니다.
	AOverPickupItem();
	// 에디터에서 배치하거나 속성을 변경할 때 배치 설정을 갱신합니다.
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Owner와 bIsHeld 도착 순서가 달라도 손 부착을 다시 적용합니다.
	virtual void OnRep_Owner() override;

	bool PickUp(USceneComponent* HoldPoint, AActor* NewOwner, FName SocketName = NAME_None);

	// 손에 든 아이템을 지정 지점에 고정하고 배치 성공 여부를 반환합니다.
	bool PlaceOn(USceneComponent* PlacementPoint, AActor* NewOwner);
	// 일반 탁자 윗면에 배치하고 다시 집을 수 있도록 조회 충돌을 켭니다.
	bool PlaceOnTable(UStaticMeshComponent* TableMesh);
	// 손에 든 아이템을 지정 위치와 회전으로 내려놓고 물리를 다시 켭니다.
	void Drop(const FVector& DropLocation, const FRotator& DropRotation);
	// 손에 들려 있거나 전용 배치 상태인 아이템은 일반 줍기 대상에서 제외합니다.
	bool CanBePickedUp() const;
	// 현재 손에 들고 있는 썰기 지원 아이템인지 확인합니다.
	bool CanBePlacedOnCuttingTable() const;
	// 배치된 재료가 썰기 가능한 원재료 상태인지 확인합니다.
	bool CanBeChopped() const;
	// 썰기 시간을 누적하며, 이번 호출로 썰기가 완료되면 true를 반환합니다.
	bool AdvanceChopping(float DeltaSeconds);
	// 재료가 썰기 완료 상태인지 반환합니다.
	bool IsChopped() const;

	// 현재 아이템을 주문 판정용 재료 정보로 변환합니다.
	virtual bool BuildPreparedIngredient(FOCPreparedIngredient& OutIngredient) const;

	virtual void Pickup_Implementation(AAPlayerCharacter* Player) override;

protected:

	virtual void BeginPlay() override;

	// 접시 메시를 지정하고 썰기 기능과 물리 시뮬레이션을 끕니다.
	void ConfigureAsPlate(UStaticMesh* PlateMesh);

	/** 파생 재료 클래스가 원본과 썰린 메시를 생성자에서 지정할 때 사용합니다. */
	void SetIngredientMeshes(UStaticMesh* RawMesh, UStaticMesh* NewChoppedMesh);
	void SetChoppingEnabled(bool bEnabled);

	// 주문 시스템에서 구분할 재료 종류입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ingredient")
	EOCIngredientType RecipeIngredientType = EOCIngredientType::None;

private:

	UFUNCTION()
	void OnRep_TransportState();

	UFUNCTION()
	void OnRep_IngredientState();

	// 동기화된 상태에 맞춰 부착·물리·충돌을 적용합니다.
	void ApplyReplicatedTransportState();

	// 월드 경계 상자의 밑면 중앙을 목표 표면 위치에 맞춥니다.
	void AlignBottomTo(const FVector& SurfaceCenter);

	// 아이템의 외형과 충돌, 물리 시뮬레이션을 담당하는 루트 메시입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	// 손에 들었을 때 아이템의 위치와 회전을 아이템 종류별로 미세 조정할 수 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	FTransform HeldTransform = FTransform::Identity;

    UPROPERTY(EditAnywhere, Category = "Pickup", meta = (ClampMin = "0.0"))
    float TableSurfaceOffset = 1.0f;

	// 썰기가 끝났을 때 교체할 메시입니다. 재료 블루프린트마다 지정할 수 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMesh> ChoppedMesh;

	// 프로젝트 설정의 원본 에셋 연결로 완성 메시를 선택합니다. 끄면 ChoppedMesh를 직접 지정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient", meta = (AllowPrivateAccess = "true"))
	bool bAutoSelectChoppedMesh = true;

	// 재료의 썰기 지원 여부이며 접시는 비활성화합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient", meta = (AllowPrivateAccess = "true"))
	bool bCanBeChopped = true;

	// 썰기를 완료하는 데 필요한 누적 시간으로 단위는 초입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ingredient", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float RequiredChopSeconds = 2.0f;


	// 현재 재료가 원재료인지 썰기 완료 상태인지 나타냅니다.
	UPROPERTY(ReplicatedUsing = OnRep_IngredientState, VisibleInstanceOnly, BlueprintReadOnly,
		Category = "Ingredient", meta = (AllowPrivateAccess = "true")
	)
	EOverIngredientState IngredientState = EOverIngredientState::Raw;

	// 현재까지 누적한 썰기 시간으로 단위는 초입니다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Ingredient", meta = (AllowPrivateAccess = "true"))
	float ChopProgressSeconds = 0.0f;



	// 캐릭터의 손에 부착되어 운반 중인지 나타냅니다.
	UPROPERTY(
		ReplicatedUsing = OnRep_TransportState, VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup",
		meta = (AllowPrivateAccess = "true")
	)
	bool bIsHeld = false;

	// 전용 지점에 고정된 상태이며 일반 탁자 배치는 이 값을 사용하지 않습니다.
	UPROPERTY(
		ReplicatedUsing = OnRep_TransportState, VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup",
		meta = (AllowPrivateAccess = "true")
	)
	bool bIsPlaced = false;

	// 일반 탁자에 고정되어 있지만 E키로 다시 집을 수 있는 상태입니다.
	UPROPERTY(ReplicatedUsing = OnRep_TransportState, VisibleInstanceOnly, BlueprintReadOnly, Category = "Pickup",
		meta = (AllowPrivateAccess = "true")
	)
	bool bIsOnTable = false;

	// 원본 에셋에 대응하는 결과 메시를 로드하며, 대응 항목이 없으면 nullptr를 반환합니다.
	UStaticMesh* FindAutomaticChoppedMesh() const;
};
