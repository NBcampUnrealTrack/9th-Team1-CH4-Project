#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OCRecipeTypes.h"
#include "OCGameMode.generated.h"

class AOCGameState;
class AOCSharedCameraActor;

UCLASS(Blueprintable)
class OVERCOOKED_API AOCGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOCGameMode();

	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Overcooked|Match")
	void StartRound(); //게임모드.라운드

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Overcooked|Score")
	void AddScore(int32 ScoreDelta); //게임모드.점수

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Overcooked|Recipe")
	bool SubmitDish(const FOCDishContents& Dish, EOCRecipeType& MatchedRecipe); //레시피.제출

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Overcooked|Debug")
	bool DebugCompleteOrderAtIndex(int32 OrderIndex); //주문.디버그

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Overcooked|Match")
	void FinishRound(); //게임모드.라운드

	UFUNCTION(BlueprintPure, Category = "Overcooked|Match")
	AOCGameState* GetOCGameState() const; //게임모드.상태
	bool CanSubmitDish(const FOCDishContents& Dish, EOCRecipeType& MatchedRecipe) const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Match", meta = (ClampMin = "0.0"))
	float CountdownDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Match", meta = (ClampMin = "1.0"))
	float RoundDuration = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Match", meta = (ClampMin = "0.0"))
	float EndingDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Score", meta = (ClampMin = "0"))
	TArray<int32> StarScoreThresholds = { 300, 600, 900 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Match")
	bool bAutoStartRound = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Network", meta = (ClampMin = "2", ClampMax = "4"))
	int32 MinimumPlayersToStart = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Order", meta = (ClampMin = "0.1"))
	float MinimumOrderInterval = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Order", meta = (ClampMin = "0.1"))
	float MaximumOrderInterval = 7.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Order", meta = (ClampMin = "1"))
	int32 MaximumActiveOrders = 6;

	// 켜면 양상추 샐러드 주문 한 개만 생성하고 완료 즉시 라운드를 종료합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Order")
	bool bUseSingleLettuceTestOrder = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Recipe")
	EOCRecipeStage RecipeStage = EOCRecipeStage::Tutorial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Score", meta = (ClampMin = "1"))
	int32 MaximumOrderTip = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Score", meta = (ClampMin = "0"))
	int32 MinimumOrderTip = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Score", meta = (ClampMin = "1.0"))
	float OrderTipDecayDuration = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Camera")
	FTransform DefaultSharedCameraTransform = FTransform(
		FRotator(-60.0f, -90.0f, 0.0f),
		FVector(1200.0f, 0.0f, 1800.0f));

	UFUNCTION(BlueprintImplementableEvent, Category = "Overcooked|Match", meta = (DisplayName = "On Countdown Started"))
	void BP_OnCountdownStarted(); //게임모드.카운트다운

	UFUNCTION(BlueprintImplementableEvent, Category = "Overcooked|Match", meta = (DisplayName = "On Round Started"))
	void BP_OnRoundStarted(); //게임모드.라운드

	UFUNCTION(BlueprintImplementableEvent, Category = "Overcooked|Match", meta = (DisplayName = "On Round Ending"))
	void BP_OnRoundEnding(); //게임모드.라운드

	UFUNCTION(BlueprintImplementableEvent, Category = "Overcooked|Match", meta = (DisplayName = "On Results Ready"))
	void BP_OnResultsReady(); //게임모드.결과

	UFUNCTION(BlueprintImplementableEvent, Category = "Overcooked|Recipe", meta = (DisplayName = "On Dish Submitted"))
	void BP_OnDishSubmitted(bool bAccepted, EOCRecipeType MatchedRecipe); //레시피.제출

private:
	int32 ResolvePlayerSlotIndex(const AController* Player) const;
	FName MakePlayerStartTag(int32 PlayerSlotIndex) const;
	bool HasEnoughPlayersToStart();
	void TryAutoStartRound();
	void RefreshParticipatingPlayerCount();
	void RefreshPlayerSlots();
	void EnsureSharedCamera();
	void ApplySharedCamera(APlayerController* PlayerController) const;
	void ApplySharedCameraToAllPlayers() const;
	void BeginPlaying();
	void ShowResults();
	void UpdateCountdown();
	void UpdateRoundTimer();
	void ConfigureRulesForCurrentMap(); //게임모드.주문
	void ScheduleNextOrder();
	void GenerateOrder();
	void AwardOrderScore(AOCGameState& State, const FOCActiveOrder& Order, bool bCompletedInOrder);
	int32 CalculateOrderTip(const AOCGameState& State, const FOCActiveOrder& Order) const;
	int32 CalculateTipMultiplier(int32 ComboCount) const;
	int32 CalculateEarnedStars(int32 FinalScore) const;
	void ClearRoundTimers();

	FTimerHandle CountdownTimerHandle;
	FTimerHandle RoundTimerHandle;
	FTimerHandle ResultsTimerHandle;
	FTimerHandle OrderTimerHandle;
	int32 NextOrderId = 0;

	UPROPERTY(Transient)
	TObjectPtr<AOCSharedCameraActor> SharedCameraActor;
};
