#include "OCGameInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

bool UOCGameInstance::HostListenServer(const FString& MapPackagePath, const int32 MaxPlayers)
{
	if (MapPackagePath.IsEmpty() || MaxPlayers < MinimumNetworkPlayers || MaxPlayers > MaximumNetworkPlayersLimit)
	{
		return false;
	}

	MaximumNetworkPlayers = MaxPlayers;
	SetPlayMode(EOCPlayMode::ListenServerHost);
	UGameplayStatics::OpenLevel(this, FName(*MapPackagePath), true, TEXT("listen"));
	return true;
}

bool UOCGameInstance::JoinListenServer(const FString& ServerAddress)
{
	if (ServerAddress.IsEmpty())
	{
		return false;
	}

	APlayerController* PrimaryController = GetFirstLocalPlayerController();
	if (!IsValid(PrimaryController))
	{
		return false;
	}

	SetPlayMode(EOCPlayMode::NetworkClient);
	PrimaryController->ClientTravel(ServerAddress, TRAVEL_Absolute);
	return true;
}

void UOCGameInstance::SetPlayMode(const EOCPlayMode NewPlayMode)
{
	if (PlayMode == NewPlayMode)
	{
		return;
	}

	PlayMode = NewPlayMode;
	OnPlayModeChanged.Broadcast(PlayMode);
}
