#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCPlayerController.generated.h"

UCLASS(Blueprintable)
class OVERCOOKED_API AOCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOCPlayerController();

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Input", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GamepadDeadZone = 0.2f;

private:
	UFUNCTION(Server, Reliable)
	void ServerDebugCompleteOrder(int32 OrderIndex);

	bool TryUseSharedCamera();
	void RefreshSharedCamera();
	FVector2D ReadMovementInput() const;
	void ApplyMovementInput(const FVector2D& MovementInput);

	FTimerHandle SharedCameraRetryTimerHandle;
};
