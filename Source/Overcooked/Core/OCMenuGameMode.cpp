#include "OCMenuGameMode.h"
#include "Engine/World.h"
#include "../UI/OCMenuPlayerController.h"
#include "Engine/World.h"

AOCMenuGameMode::AOCMenuGameMode()
{
	bUseSeamlessTravel = true;

	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
	
	PlayerControllerClass = AOCMenuPlayerController::StaticClass();
}

void AOCMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UpdatePlayerSlots();
}
void AOCMenuGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UpdatePlayerSlots();
}

void AOCMenuGameMode::StartGame(APlayerController* RequestingPlayer)
{
	if (!HasAuthority() || !IsValid(RequestingPlayer) || !RequestingPlayer->IsLocalController())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("MENU START - NetMode=%d Players=%d Map=%s"),
		static_cast<int32>(GetNetMode()),
		GetNumPlayers(),
		*World->GetMapName());

	const FString TravelURL = FString::Printf(
		TEXT("/Game/Overcooked/Maps/DevMap?ExpectedPlayers=%d"),
		FMath::Max(GetNumPlayers(), 1));
	World->ServerTravel(TravelURL);
}
void AOCMenuGameMode::UpdatePlayerSlots()
{
	const int32 PlayerCount = GetNumPlayers();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
		 It;
		 ++It)
	{
		if (AOCMenuPlayerController* MenuPC =
			Cast<AOCMenuPlayerController>(It->Get()))
		{
			MenuPC->ClientUpdatePlayerSlots(PlayerCount);
		}
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("MENU PLAYER SLOTS UPDATED - Players=%d"),
		PlayerCount
	);
}
