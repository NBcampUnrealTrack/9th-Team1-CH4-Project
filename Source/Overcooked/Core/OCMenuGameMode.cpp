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
void AOCMenuGameMode::StartGame()
{
	if (!HasAuthority())
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

	World->ServerTravel(TEXT("/Game/Overcooked/Maps/DevMap"));
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