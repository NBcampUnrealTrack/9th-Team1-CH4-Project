#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OCMatchTypes.h"
#include "OCRecipeTypes.h"
#include "OCGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOCMatchPhaseChangedSignature, EOCMatchPhase, NewPhase, EOCMatchPhase, PreviousPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOCRemainingTimeChangedSignature, float, NewRemainingTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOCScoreChangedSignature, int32, NewScore, int32, ScoreDelta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOCMatchResultReadySignature, const FOCMatchResult&, MatchResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOCParticipatingPlayerCountChangedSignature, int32, NewPlayerCount, int32, PreviousPlayerCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOCOrdersChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOCComboChangedSignature, int32, NewComboCount, int32, NewTipMultiplier);

UCLASS(BlueprintType)
class OVERCOOKED_API AOCGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AOCGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Overcooked|Match")
	EOCMatchPhase GetMatchPhase() const { return MatchPhase; } //게임상태.진행

	UFUNCTION(BlueprintPure, Category = "Overcooked|Match")
	float GetRemainingTime() const { return RemainingTime; } //게임상태.시간

	UFUNCTION(BlueprintPure, Category = "Overcooked|Score")
	int32 GetCurrentScore() const { return CurrentScore; } //게임상태.점수

	UFUNCTION(BlueprintPure, Category = "Overcooked|Match")
	FOCMatchResult GetMatchResult() const { return MatchResult; } //게임상태.결과

	UFUNCTION(BlueprintPure, Category = "Overcooked|Network")
	int32 GetParticipatingPlayerCount() const { return ParticipatingPlayerCount; } //게임상태.인원

	UFUNCTION(BlueprintPure, Category = "Overcooked|Order")
	TArray<FOCActiveOrder> GetActiveOrders() const { return ActiveOrders; } //주문.목록

	const TArray<FOCActiveOrder>& GetActiveOrdersRef() const { return ActiveOrders; } //주문.목록

	UFUNCTION(BlueprintPure, Category = "Overcooked|Score")
	int32 GetComboCount() const { return ComboCount; } //점수.콤보

	UFUNCTION(BlueprintPure, Category = "Overcooked|Score")
	int32 GetTipMultiplier() const { return TipMultiplier; } //점수.팁

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCMatchPhaseChangedSignature OnMatchPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCRemainingTimeChangedSignature OnRemainingTimeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCScoreChangedSignature OnScoreChanged;

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCMatchResultReadySignature OnMatchResultReady;

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCParticipatingPlayerCountChangedSignature OnParticipatingPlayerCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCOrdersChangedSignature OnOrdersChanged;

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCComboChangedSignature OnComboChanged;

	void SetMatchPhase(EOCMatchPhase NewPhase);
	void SetRemainingTime(float NewRemainingTime);
	void SetCurrentScore(int32 NewScore);
	void SetMatchResult(const FOCMatchResult& NewResult);
	void SetParticipatingPlayerCount(int32 NewPlayerCount);
	void AddOrder(const FOCActiveOrder& NewOrder);
	bool RemoveOrder(int32 OrderId);
	void ClearOrders();
	void SetCombo(int32 NewComboCount, int32 NewTipMultiplier);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Match")
	EOCMatchPhase MatchPhase = EOCMatchPhase::Waiting;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Match")
	float RemainingTime = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentScore, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Score")
	int32 CurrentScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchResult, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Match")
	FOCMatchResult MatchResult;

	UPROPERTY(ReplicatedUsing = OnRep_ParticipatingPlayerCount, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Network")
	int32 ParticipatingPlayerCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveOrders, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Order")
	TArray<FOCActiveOrder> ActiveOrders;

	UPROPERTY(ReplicatedUsing = OnRep_Combo, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Score")
	int32 ComboCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_Combo, VisibleInstanceOnly, BlueprintReadOnly, Category = "Overcooked|Score")
	int32 TipMultiplier = 1;

private:
	UFUNCTION()
	void OnRep_MatchPhase(EOCMatchPhase PreviousPhase);

	UFUNCTION()
	void OnRep_RemainingTime(float PreviousRemainingTime);

	UFUNCTION()
	void OnRep_CurrentScore(int32 PreviousScore);

	UFUNCTION()
	void OnRep_MatchResult();

	UFUNCTION()
	void OnRep_ParticipatingPlayerCount(int32 PreviousPlayerCount);

	UFUNCTION()
	void OnRep_ActiveOrders();

	UFUNCTION()
	void OnRep_Combo();
};
