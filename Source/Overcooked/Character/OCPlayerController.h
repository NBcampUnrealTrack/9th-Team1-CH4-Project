#pragma once

#include "CoreMinimal.h"
#include "../Core/OCRecipeTypes.h"
#include "GameFramework/PlayerController.h"
#include "OCPlayerController.generated.h"

class UTexture2D;
class UOCHUDWidget;
class UOCResultWidget;
class UOCTutorialWidget;
class UUserWidget;
class USoundBase;
class UAudioComponent;
UCLASS(Blueprintable)
class OVERCOOKED_API AOCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOCPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PlayerTick(float DeltaTime) override;

	void ConfigureSinglePlayerCharacters(APawn* PrimaryCharacter, APawn* SecondaryCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "Overcooked|Game")
	void StartGame();
		UFUNCTION(Server, Reliable)
	void ServerFinishTutorial();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RequestMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RequestNextStage();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RequestRetryStage();

	UFUNCTION(Server, Reliable)
	void ServerRequestRetryStage();
	
	UFUNCTION(Server, Reliable)
	void ServerRequestMainMenu();

	UFUNCTION(Server, Reliable)
	void ServerRequestNextStage();
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void UpdateBGMSpeed(float RemainingTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> GameBGM;
	UPROPERTY()
	TObjectPtr<UAudioComponent> GameBGMComponent;

	bool bBGMSpedUp = false;
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI")
	TSubclassOf<UOCHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UOCHUDWidget> HUDWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI")
	TMap<EOCRecipeType, TObjectPtr<UTexture2D>> OrderCardTextures;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Input", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float GamepadDeadZone = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI")
    TSubclassOf<UOCResultWidget> ResultWidgetClass;

    UPROPERTY()
    TObjectPtr<UOCResultWidget> ResultWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI")
	TSubclassOf<UOCTutorialWidget> TutorialWidgetClass;

	UPROPERTY()
	TObjectPtr<UOCTutorialWidget> TutorialWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI|Tutorial")
	TObjectPtr<UTexture2D> TutorialBasicTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI|Tutorial")
	TObjectPtr<UTexture2D> TutorialStage1Texture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI|Tutorial")
	TObjectPtr<UTexture2D> TutorialStage2Texture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI|Tutorial")
	TObjectPtr<UTexture2D> TutorialStage3Texture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|UI")
	TSubclassOf<UUserWidget> TimeOverWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> TimeOverWidget;
private:
	UFUNCTION(Server, Reliable)
	void ServerDebugCompleteOrder(int32 OrderIndex);

	UFUNCTION(Server, Reliable)
	void ServerSwitchSinglePlayerCharacter();

	UFUNCTION()
	void HandleScoreChanged(int32 NewScore, int32 ScoreDelta);
	UFUNCTION()
	void HandleRemainingTimeChanged(float NewRemainingTime);
	UFUNCTION()
	void HandleComboChanged(int32 NewComboCount, int32 NewTipMultiplier);
	UFUNCTION()
	void HandleOrdersChanged();	
	UFUNCTION()
	void HandleMatchResultReady(const FOCMatchResult& MatchResult);
	UFUNCTION()
	void HandleMatchPhaseChanged(
		EOCMatchPhase NewPhase,
		EOCMatchPhase PreviousPhase
	);
	
	bool TryUseSharedCamera();
	void BindGameState();
	void RefreshSharedCamera();
	void EnsureDefaultOrderCardTextures(); //UI.주문
	FVector2D ReadMovementInput() const;
	void ApplyMovementInput(const FVector2D& MovementInput);

	FTimerHandle SharedCameraRetryTimerHandle;
	FTimerHandle GameStateRetryTimerHandle;
	FTimerHandle ResultDelayTimerHandle;
	bool bResultPresentationStarted = false;

	UPROPERTY()
	TObjectPtr<APawn> SinglePlayerPrimaryCharacter;

	UPROPERTY()
	TObjectPtr<APawn> SinglePlayerSecondaryCharacter;
};
