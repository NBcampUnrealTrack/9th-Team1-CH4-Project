#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OCPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOCPlayerSlotChangedSignature, int32, NewSlotIndex, int32, PreviousSlotIndex);

UCLASS(BlueprintType)
class OVERCOOKED_API AOCPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AOCPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Overcooked|Player")
	int32 GetPlayerSlotIndex() const { return PlayerSlotIndex; } //플레이어.슬롯

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCPlayerSlotChangedSignature OnPlayerSlotChanged;

	void SetPlayerSlotIndex(int32 NewSlotIndex);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSlotIndex, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Player")
	int32 PlayerSlotIndex = INDEX_NONE;

private:
	UFUNCTION()
	void OnRep_PlayerSlotIndex(int32 PreviousSlotIndex);
};
