// 프로젝트 설정의 Description 페이지에서 저작권 문구를 설정할 수 있습니다.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interaction/InteractableInterface.h"
#include "OverCuttingTable.generated.h"

class AOverPickupItem;
class USceneComponent;
class UStaticMeshComponent;
class AAPlayerCharacter;

/** 재료 한 개를 올려놓고 썰 수 있는 작업대입니다. */
UCLASS(Blueprintable)
class OVERCOOKED_API AOverCuttingTable : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	// 작업대 메시와 재료 한 개를 올려놓을 부착 지점을 생성합니다.
	AOverCuttingTable();

	// 에디터에서 배치하거나 속성을 변경할 때 배치 설정을 갱신합니다.
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Interact_Implementation(AAPlayerCharacter* Player) override;

	virtual void StopInteract_Implementation(AAPlayerCharacter* Player) override;

	// 작업대가 비어 있고 재료를 배치할 수 있을 때 부착 후 보관합니다.
	bool PlaceIngredient(AOverPickupItem* Ingredient);
	// 작업대의 재료를 손으로 옮기며, 실패하거나 재료가 없으면 nullptr를 반환합니다.
	AOverPickupItem* TakeIngredient(USceneComponent* HoldPoint, AActor* NewOwner);
	// 작업대가 비어 있고 전달된 재료를 올릴 수 있는지 확인합니다.
	bool CanPlaceIngredient(const AOverPickupItem* Ingredient) const;
	// 작업대 위에 썰 수 있는 재료가 있는지 확인합니다.
	bool CanChopIngredient() const;
	// 썰기 시간을 누적하며, 이번 호출로 썰기가 완료되면 true를 반환합니다.
	bool AdvanceChopping(float DeltaSeconds);
	// 상호작용 거리 판정에 사용할 배치 지점의 월드 위치를 반환합니다.
	FVector GetInteractionLocation() const;

protected:
	virtual void BeginPlay() override;

private:

	// F키를 누르고 있는 동안 서버에서 썰기 시간을 누적합니다.
	void AdvanceChoppingTick();

	// 썰기 시간을 누적할 주기입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutting Table",
		meta = (ClampMin = "0.01", AllowPrivateAccess = "true"))
	float ChopTickInterval = 0.1f;

	// 현재 이 도마를 사용해 썰고 있는 플레이어입니다.
	TWeakObjectPtr<AAPlayerCharacter> ChoppingPlayer;

	FTimerHandle ChoppingTimerHandle;

	// 작업대의 외형과 충돌을 담당하는 루트 메시입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cutting Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> TableMesh;

	// 작업대 윗면에서 재료를 부착할 기준점입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cutting Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> IngredientPoint;

	// 메시 윗면 중앙을 기준으로 재료 배치 위치를 미세 조정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cutting Table", meta = (AllowPrivateAccess = "true"))
	FVector IngredientPointOffset = FVector(0.0f, 0.0f, 5.0f);

	// 현재 작업대에 놓인 재료이며 비어 있으면 유효한 참조가 없습니다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cutting Table", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AOverPickupItem> PlacedIngredient;
};
