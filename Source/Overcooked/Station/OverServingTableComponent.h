#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OverServingTableComponent.generated.h"

class AOverPickupItem;
class AOverPlateItem;
class UStaticMesh;
class UMaterialInterface;
class UStaticMeshComponent;
class USceneComponent;

/** 기존 탁자에 서빙 기능을 추가하고 접시 제거와 빈 접시 반환을 관리합니다. */
UCLASS(ClassGroup = (Overcooked), meta = (BlueprintSpawnableComponent))
class OVERCOOKED_API UOverServingTableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 타이머로 서빙을 처리하므로 매 프레임 갱신을 비활성화합니다.
	UOverServingTableComponent();
	// 음식이 담긴 접시를 받아 반환 위치를 저장하고 정리 타이머를 시작합니다.
	bool Serve(AOverPickupItem* Ingredient);
	// 상호작용 거리 판정에 사용할 배치 지점의 월드 위치를 반환합니다.
	FVector GetInteractionLocation() const;

protected:
	// 게임 시작 시 필요한 배치와 구성 요소를 준비합니다.
	virtual void BeginPlay() override;
	// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 서빙 접시를 받은 뒤 제거하기까지 기다리는 시간으로 단위는 초입니다.
	UPROPERTY(EditAnywhere, Category = "Serving", meta = (ClampMin = "0.0"))
	float ClearDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Serving", meta = (ClampMin = "0.0"))
	float SurfaceOffset = 1.0f;

	// 서빙 접시를 제거한 뒤 빈 접시 반환까지 기다리는 시간으로 단위는 초입니다.
	UPROPERTY(EditAnywhere, Category = "Serving", meta = (ClampMin = "0.0"))
	float PlateRespawnDelay = 1.0f;

private:
	// 서빙된 접시와 음식을 제거하고 빈 접시 반환 타이머를 시작합니다.
	void ClearServing();
	// 저장된 원래 위치에 빈 접시 생성을 시도한 뒤 다음 서빙을 허용합니다.
	void RestorePlate();

	// 받은 접시에서 복사한 빈 접시 재생성용 변환입니다.
	FTransform PlateReturnTransform;

	UPROPERTY(Transient)
	TSubclassOf<AOverPlateItem> ReturnedPlateClass;

	// 인스턴스에서 지정한 접시 메시와 재질도 반환 시 유지합니다.
	UPROPERTY(Transient)
	TObjectPtr<UStaticMesh> ReturnedPlateMesh;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInterface>> ReturnedPlateMaterials;

	// 음식이나 서빙 접시를 부착하는 기준점입니다.
	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> FoodPoint;

	// 현재 서빙 중인 접시 액터를 보관합니다.
	UPROPERTY(Transient)
	TObjectPtr<AOverPickupItem> ServedIngredient;

	// 접시 제거와 반환을 순서대로 예약하는 일회성 타이머 핸들입니다.
	FTimerHandle ServingTimer;
	// 초기화와 이전 접시 반환 대기가 끝나 새 서빙을 받을 수 있는지 나타냅니다.
	bool bReady = false;
};
