#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCMenuPlayerController.generated.h"


class UOCMenuWidget;

UCLASS()
class OVERCOOKED_API AOCMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOCMenuPlayerController();
	
	UFUNCTION(Client, Reliable)
	void ClientUpdatePlayerSlots(int32 PlayerCount);
	UFUNCTION(Server, Reliable)
	void ServerRequestPlayerSlots();
	
private:
	UOCMenuWidget* FindMenuWidget() const;
};