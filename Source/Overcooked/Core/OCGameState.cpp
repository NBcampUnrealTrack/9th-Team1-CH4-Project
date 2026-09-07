#include "OCGameState.h"

#include "Net/UnrealNetwork.h"

AOCGameState::AOCGameState()
{
	bReplicates = true;
}

void AOCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOCGameState, MatchPhase);
	DOREPLIFETIME(AOCGameState, RemainingTime);
	DOREPLIFETIME(AOCGameState, CurrentScore);
	DOREPLIFETIME(AOCGameState, MatchResult);
	DOREPLIFETIME(AOCGameState, ParticipatingPlayerCount);
	DOREPLIFETIME(AOCGameState, ActiveOrders);
}

void AOCGameState::SetMatchPhase(const EOCMatchPhase NewPhase)
{
	if (!HasAuthority() || MatchPhase == NewPhase)
	{
		return;
	}

	const EOCMatchPhase PreviousPhase = MatchPhase;
	MatchPhase = NewPhase;
	OnMatchPhaseChanged.Broadcast(MatchPhase, PreviousPhase);
	ForceNetUpdate();
}

void AOCGameState::SetRemainingTime(const float NewRemainingTime)
{
	if (!HasAuthority())
	{
		return;
	}

	const float ClampedTime = FMath::Max(0.0f, NewRemainingTime);
	if (FMath::IsNearlyEqual(RemainingTime, ClampedTime))
	{
		return;
	}

	RemainingTime = ClampedTime;
	OnRemainingTimeChanged.Broadcast(RemainingTime);
	ForceNetUpdate();
}

void AOCGameState::SetCurrentScore(const int32 NewScore)
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 ClampedScore = FMath::Max(0, NewScore);
	if (CurrentScore == ClampedScore)
	{
		return;
	}

	const int32 ScoreDelta = ClampedScore - CurrentScore;
	CurrentScore = ClampedScore;
	OnScoreChanged.Broadcast(CurrentScore, ScoreDelta);
	ForceNetUpdate();
}

void AOCGameState::SetMatchResult(const FOCMatchResult& NewResult)
{
	if (!HasAuthority())
	{
		return;
	}

	MatchResult = NewResult;
	OnMatchResultReady.Broadcast(MatchResult);
	ForceNetUpdate();
}

void AOCGameState::SetParticipatingPlayerCount(const int32 NewPlayerCount)
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 ClampedCount = FMath::Clamp(NewPlayerCount, 0, 4);
	if (ParticipatingPlayerCount == ClampedCount)
	{
		return;
	}

	const int32 PreviousPlayerCount = ParticipatingPlayerCount;
	ParticipatingPlayerCount = ClampedCount;
	OnParticipatingPlayerCountChanged.Broadcast(ParticipatingPlayerCount, PreviousPlayerCount);
	ForceNetUpdate();
}

void AOCGameState::AddOrder(const FOCActiveOrder& NewOrder)
{
	if (!HasAuthority() || NewOrder.Recipe == EOCRecipeType::None)
	{
		return;
	}

	ActiveOrders.Add(NewOrder);
	OnOrdersChanged.Broadcast();
	ForceNetUpdate();
}

bool AOCGameState::RemoveOrder(const int32 OrderId)
{
	if (!HasAuthority())
	{
		return false;
	}

	const int32 RemovedCount = ActiveOrders.RemoveAll(
		[OrderId](const FOCActiveOrder& Order) { return Order.OrderId == OrderId; });
	if (RemovedCount > 0)
	{
		OnOrdersChanged.Broadcast();
		ForceNetUpdate();
	}

	return RemovedCount > 0;
}

void AOCGameState::ClearOrders()
{
	if (!HasAuthority() || ActiveOrders.IsEmpty())
	{
		return;
	}

	ActiveOrders.Reset();
	OnOrdersChanged.Broadcast();
	ForceNetUpdate();
}

void AOCGameState::OnRep_MatchPhase(const EOCMatchPhase PreviousPhase)
{
	OnMatchPhaseChanged.Broadcast(MatchPhase, PreviousPhase);
}

void AOCGameState::OnRep_RemainingTime(const float PreviousRemainingTime)
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void AOCGameState::OnRep_CurrentScore(const int32 PreviousScore)
{
	OnScoreChanged.Broadcast(CurrentScore, CurrentScore - PreviousScore);
}

void AOCGameState::OnRep_MatchResult()
{
	OnMatchResultReady.Broadcast(MatchResult);
}

void AOCGameState::OnRep_ParticipatingPlayerCount(const int32 PreviousPlayerCount)
{
	OnParticipatingPlayerCountChanged.Broadcast(ParticipatingPlayerCount, PreviousPlayerCount);
}

void AOCGameState::OnRep_ActiveOrders()
{
	OnOrdersChanged.Broadcast();
}
