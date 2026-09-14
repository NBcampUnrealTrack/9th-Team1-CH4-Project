#pragma once

#include "CoreMinimal.h"
#include "OverPickupItem.h"


#include "OverPlateItem.generated.h"

/** 음식을 담아 운반하고 서빙할 수 있는 접시입니다. */
UCLASS()
class OVERCOOKED_API AOverPlateItem : public AOverPickupItem
{
	GENERATED_BODY()

public:
	// 접시 메시와 음식을 부착할 기준점을 생성합니다.
	AOverPlateItem();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// 들고 있는 재료를 접시에 추가하고 성공 여부를 반환합니다.
	bool AddFood(AOverPickupItem* Ingredient);
	// 접시에 담긴 음식 액터가 하나 이상 유효한지 확인합니다.
	bool HasFood() const;

	// 접시에 담긴 재료를 주문 제출용 데이터로 만듭니다.
	bool BuildDishContents(FOCDishContents& OutDish) const;
	// 탁자 위 접시는 캐릭터에 가까운 탁자 경계를 거리 판정 위치로 사용합니다.
	FVector GetPickupReachLocation(const FVector& CharacterLocation) const;
	// 서빙 후 빈 접시가 다시 생성될 원래 위치, 회전, 크기입니다.
	FTransform ReturnTransform;

	virtual void Interact_Implementation(AAPlayerCharacter* Player) override;

protected:
	virtual void BeginPlay() override;

	// 종료 시 부착된 음식이나 서빙 접시 등 관련 자원을 정리합니다.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnRep_DisplayedRecipe();

	void RefreshCompletedDishVisual();
	UStaticMesh* GetCompletedDishMesh(EOCRecipeType Recipe) const;

    UPROPERTY(EditAnywhere, Category = "Plate", meta = (ClampMin = "0.0"))
    float SurfaceTracePadding = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Plate", meta = (ClampMin = "0.0"))
    float SupportTraceStartOffset = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Plate", meta = (ClampMin = "0.0"))
    float SupportTraceDepth = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Plate")
    TEnumAsByte<ECollisionChannel> SupportTraceChannel = ECC_Visibility;

	// 음식이나 서빙 접시를 부착하는 기준점입니다.
	UPROPERTY()
	TObjectPtr<USceneComponent> FoodPoint;

	UPROPERTY(VisibleAnywhere, Category = "Plate")
	TObjectPtr<UStaticMeshComponent> CompletedDishMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> ShrimpNigiriMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> OctopusNigiriMesh;

	UPROPERTY()
	TObjectPtr<UStaticMesh> SalmonNigiriMesh;

	UPROPERTY(ReplicatedUsing = OnRep_DisplayedRecipe)
	EOCRecipeType DisplayedRecipe = EOCRecipeType::None;

	UPROPERTY(EditAnywhere, Category = "Plate", meta = (ClampMin = "1", ClampMax = "6"))
	int32 MaximumFoodCount = 4;

	// 접시에 담긴 음식들을 보관합니다.
	UPROPERTY()
	TArray<TObjectPtr<AOverPickupItem>> Foods;
};
