#include "OCMenuGameMode.h"
#include "Engine/World.h"

AOCMenuGameMode::AOCMenuGameMode()
{
	bUseSeamlessTravel = true;

	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
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