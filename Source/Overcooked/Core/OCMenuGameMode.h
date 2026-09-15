#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OCMenuGameMode.generated.h"

UCLASS()
class OVERCOOKED_API AOCMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOCMenuGameMode();

	UFUNCTION(BlueprintCallable, Category = "Overcooked|Menu")
	void StartGame(APlayerController* RequestingPlayer);
	void UpdatePlayerSlots();
	protected:
        virtual void PostLogin(APlayerController* NewPlayer) override;
		virtual void Logout(AController* Exiting) override;
    
    private:
       
};
