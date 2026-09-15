

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERCOOKED_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();

	void TryInteract();
	void StopInteract();
	void TryPickup();
	// E키로 일반 탁자 배치를 처리하며, 탁자를 찾았거나 자리가 차 있으면 true를 반환합니다.
	bool TryPlaceHeldItemOnTable();

private:
	// F키를 누르기 시작했을 때 상호작용한 대상을 기억합니다.
	TWeakObjectPtr<AActor> ActiveInteractTarget;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionDistance = 150.0f;


	// 캐릭터 정면에서 픽업 구체 중심까지의 거리입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Pickup", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float PickupSphereForwardOffset = 100.0f;

	// 캐릭터 캡슐 바닥에서 픽업 구체 중심까지의 높이입니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Pickup", meta = (ClampMin = "0.0", AllowPrivateAccess = "true"))
	float PickupSphereHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Pickup", meta = (ClampMin = "1.0", AllowPrivateAccess = "true"))
	float PickupSphereRadius = 110.0f;

	// 켜면 픽업 입력 시 실제 검사 범위를 표시합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDrawPickupDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Debug", meta = (ClampMin = "0.0", EditCondition = "bDrawPickupDebug", AllowPrivateAccess = "true"))
	float PickupDebugDuration = 1.5f;
};
