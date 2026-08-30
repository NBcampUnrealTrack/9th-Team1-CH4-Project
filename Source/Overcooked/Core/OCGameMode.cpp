#include "OCGameMode.h"

#include "OCGameInstance.h"
#include "OCGameState.h"
#include "OCPlayerState.h"
#include "../Camera/OCSharedCameraActor.h"
#include "../Character/OCCharacter.h"
#include "../Character/OCPlayerController.h"
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogOCGameMode, Log, All);

AOCGameMode::AOCGameMode()
{
	GameStateClass = AOCGameState::StaticClass();
	PlayerStateClass = AOCPlayerState::StaticClass();
	DefaultPawnClass = AOCCharacter::StaticClass();
	PlayerControllerClass = AOCPlayerController::StaticClass();
}

void AOCGameMode::StartPlay()
{
	Super::StartPlay();

	EnsureSharedCamera();
	ApplySharedCameraToAllPlayers();
	RefreshParticipatingPlayerCount();
	RefreshPlayerSlots();
	TryAutoStartRound();
}

void AOCGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ApplySharedCamera(NewPlayer);
	RefreshParticipatingPlayerCount();
	RefreshPlayerSlots();
	TryAutoStartRound();
}

void AOCGameMode::PreLogin(
	const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!ErrorMessage.IsEmpty())
	{
		return;
	}

	const UOCGameInstance* OCGameInstance = GetGameInstance<UOCGameInstance>();
	if (IsValid(OCGameInstance)
		&& GetNetMode() == NM_ListenServer
		&& GetNumPlayers() >= OCGameInstance->GetMaximumNetworkPlayers())
	{
		ErrorMessage = TEXT("Server is full.");
	}
}

void AOCGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	RefreshParticipatingPlayerCount();
	RefreshPlayerSlots();
}

AActor* AOCGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	const int32 PlayerSlotIndex = ResolvePlayerSlotIndex(Player);
	if (PlayerSlotIndex != INDEX_NONE)
	{
		const FName RequiredTag = MakePlayerStartTag(PlayerSlotIndex);
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			if (It->PlayerStartTag == RequiredTag)
			{
				return *It;
			}
		}

		UE_LOG(
			LogOCGameMode,
			Warning,
			TEXT("Player slot %d could not find a PlayerStart with Player Start Tag '%s'. Falling back to Unreal's default selection."),
			PlayerSlotIndex,
			*RequiredTag.ToString());
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void AOCGameMode::StartRound()
{
	if (!HasAuthority())
	{
		return;
	}

	AOCGameState* State = GetOCGameState();
	if (!IsValid(State))
	{
		return;
	}

	ClearRoundTimers();
	State->SetCurrentScore(0);
	State->SetRemainingTime(CountdownDuration);
	State->SetMatchPhase(EOCMatchPhase::Countdown);
	BP_OnCountdownStarted();

	if (CountdownDuration <= 0.0f)
	{
		BeginPlaying();
		return;
	}

	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle,
		this,
		&AOCGameMode::UpdateCountdown,
		0.1f,
		true);
}

void AOCGameMode::AddScore(const int32 ScoreDelta)
{
	AOCGameState* State = GetOCGameState();
	if (!HasAuthority() || !IsValid(State) || State->GetMatchPhase() != EOCMatchPhase::Playing)
	{
		return;
	}

	State->SetCurrentScore(State->GetCurrentScore() + ScoreDelta);
}

void AOCGameMode::FinishRound()
{
	AOCGameState* State = GetOCGameState();
	if (!HasAuthority() || !IsValid(State))
	{
		return;
	}

	const EOCMatchPhase Phase = State->GetMatchPhase();
	if (Phase == EOCMatchPhase::Ending || Phase == EOCMatchPhase::Results)
	{
		return;
	}

	ClearRoundTimers();
	State->SetRemainingTime(0.0f);
	State->SetMatchPhase(EOCMatchPhase::Ending);
	BP_OnRoundEnding();

	if (EndingDuration <= 0.0f)
	{
		ShowResults();
		return;
	}

	GetWorldTimerManager().SetTimer(
		ResultsTimerHandle,
		this,
		&AOCGameMode::ShowResults,
		EndingDuration,
		false);
}

AOCGameState* AOCGameMode::GetOCGameState() const
{
	return GetGameState<AOCGameState>();
}

int32 AOCGameMode::ResolvePlayerSlotIndex(const AController* Player) const
{
	if (!IsValid(Player))
	{
		return INDEX_NONE;
	}

	if (const AOCPlayerState* OCPlayerState = Player->GetPlayerState<AOCPlayerState>())
	{
		if (OCPlayerState->GetPlayerSlotIndex() != INDEX_NONE)
		{
			return OCPlayerState->GetPlayerSlotIndex();
		}
	}

	const AOCGameState* State = GetOCGameState();
	const APlayerState* BasePlayerState = Player->GetPlayerState<APlayerState>();
	const int32 PlayerArrayIndex = IsValid(State) ? State->PlayerArray.IndexOfByKey(BasePlayerState) : INDEX_NONE;
	if (PlayerArrayIndex >= 0 && PlayerArrayIndex < 4)
	{
		return PlayerArrayIndex;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Player);
	const ULocalPlayer* LocalPlayer = IsValid(PlayerController) ? PlayerController->GetLocalPlayer() : nullptr;
	const int32 ControllerId = IsValid(LocalPlayer) ? LocalPlayer->GetControllerId() : INDEX_NONE;
	return ControllerId >= 0 && ControllerId < 4 ? ControllerId : INDEX_NONE;
}

FName AOCGameMode::MakePlayerStartTag(const int32 PlayerSlotIndex) const
{
	return FName(*FString::Printf(TEXT("PlayerStart%d"), PlayerSlotIndex + 1));
}

bool AOCGameMode::HasEnoughPlayersToStart()
{
	const UOCGameInstance* OCGameInstance = GetGameInstance<UOCGameInstance>();
	if (!IsValid(OCGameInstance))
	{
		return false;
	}

	return GetNumPlayers() >= MinimumPlayersToStart;
}

void AOCGameMode::TryAutoStartRound()
{
	AOCGameState* State = GetOCGameState();
	if (bAutoStartRound
		&& IsValid(State)
		&& State->GetMatchPhase() == EOCMatchPhase::Waiting
		&& HasEnoughPlayersToStart())
	{
		StartRound();
	}
}

void AOCGameMode::RefreshParticipatingPlayerCount()
{
	AOCGameState* State = GetOCGameState();
	if (IsValid(State))
	{
		State->SetParticipatingPlayerCount(GetNumPlayers());
	}
}

void AOCGameMode::RefreshPlayerSlots()
{
	AOCGameState* State = GetOCGameState();
	if (!HasAuthority() || !IsValid(State))
	{
		return;
	}

	for (int32 SlotIndex = 0; SlotIndex < State->PlayerArray.Num(); ++SlotIndex)
	{
		if (AOCPlayerState* OCPlayerState = Cast<AOCPlayerState>(State->PlayerArray[SlotIndex]))
		{
			OCPlayerState->SetPlayerSlotIndex(SlotIndex);
		}
	}
}

void AOCGameMode::EnsureSharedCamera()
{
	for (TActorIterator<AOCSharedCameraActor> It(GetWorld()); It; ++It)
	{
		SharedCameraActor = *It;
		break;
	}

	if (!IsValid(SharedCameraActor))
	{
		SharedCameraActor = GetWorld()->SpawnActor<AOCSharedCameraActor>(
			AOCSharedCameraActor::StaticClass(),
			DefaultSharedCameraTransform);
	}
}

void AOCGameMode::ApplySharedCamera(APlayerController* PlayerController) const
{
	if (IsValid(PlayerController) && IsValid(SharedCameraActor))
	{
		PlayerController->SetViewTarget(SharedCameraActor);
	}
}

void AOCGameMode::ApplySharedCameraToAllPlayers() const
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ApplySharedCamera(It->Get());
	}
}

void AOCGameMode::BeginPlaying()
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	AOCGameState* State = GetOCGameState();
	if (!IsValid(State))
	{
		return;
	}

	State->SetRemainingTime(RoundDuration);
	State->SetMatchPhase(EOCMatchPhase::Playing);
	BP_OnRoundStarted();

	GetWorldTimerManager().SetTimer(
		RoundTimerHandle,
		this,
		&AOCGameMode::UpdateRoundTimer,
		0.1f,
		true);
}

void AOCGameMode::ShowResults()
{
	GetWorldTimerManager().ClearTimer(ResultsTimerHandle);

	AOCGameState* State = GetOCGameState();
	if (!IsValid(State))
	{
		return;
	}

	FOCMatchResult Result;
	Result.FinalScore = State->GetCurrentScore();
	Result.EarnedStars = CalculateEarnedStars(Result.FinalScore);
	State->SetMatchResult(Result);
	State->SetMatchPhase(EOCMatchPhase::Results);
	BP_OnResultsReady();
}

void AOCGameMode::UpdateCountdown()
{
	AOCGameState* State = GetOCGameState();
	if (!IsValid(State))
	{
		return;
	}

	const float NewTime = State->GetRemainingTime() - GetWorldTimerManager().GetTimerRate(CountdownTimerHandle);
	State->SetRemainingTime(NewTime);

	if (NewTime <= 0.0f)
	{
		BeginPlaying();
	}
}

void AOCGameMode::UpdateRoundTimer()
{
	AOCGameState* State = GetOCGameState();
	if (!IsValid(State))
	{
		return;
	}

	const float NewTime = State->GetRemainingTime() - GetWorldTimerManager().GetTimerRate(RoundTimerHandle);
	State->SetRemainingTime(NewTime);

	if (NewTime <= 0.0f)
	{
		FinishRound();
	}
}

int32 AOCGameMode::CalculateEarnedStars(const int32 FinalScore) const
{
	int32 EarnedStars = 0;
	for (const int32 Threshold : StarScoreThresholds)
	{
		if (FinalScore >= Threshold)
		{
			++EarnedStars;
		}
	}

	return EarnedStars;
}

void AOCGameMode::ClearRoundTimers()
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	GetWorldTimerManager().ClearTimer(ResultsTimerHandle);
}
