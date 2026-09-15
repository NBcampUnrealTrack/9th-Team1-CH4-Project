#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OCSessionManager.h"
#include "OCMenuPlayerController.generated.h"


class UOCMenuWidget;
class UOCSessionManager;
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
	UFUNCTION(BlueprintCallable, Category = "Overcooked|Session")
	void HostGame();
	UFUNCTION(BlueprintCallable, Category = "Overcooked|Session")
	void JoinGame();
protected:
	virtual void BeginPlay() override;
private:
	UOCMenuWidget* FindMenuWidget() const;
	UPROPERTY()
	TObjectPtr<UOCSessionManager> SessionManager;
};