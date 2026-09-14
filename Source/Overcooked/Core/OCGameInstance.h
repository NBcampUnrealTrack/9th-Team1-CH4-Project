#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OCGameInstance.generated.h"

UENUM(BlueprintType)
enum class EOCPlayMode : uint8
{
	ListenServerHost UMETA(DisplayName = "Listen Server Host"),
	NetworkClient	UMETA(DisplayName = "Network Client")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOCPlayModeChangedSignature, EOCPlayMode, NewPlayMode);

UCLASS(BlueprintType)
class OVERCOOKED_API UOCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	static constexpr int32 MinimumNetworkPlayers = 2;
	static constexpr int32 MaximumNetworkPlayersLimit = 4;

	UFUNCTION(BlueprintCallable, Category = "Overcooked|Network")
	bool HostListenServer(const FString& MapPackagePath, int32 MaxPlayers = 4); //네트워크.호스트

	UFUNCTION(BlueprintCallable, Category = "Overcooked|Network")
	bool JoinListenServer(const FString& ServerAddress); //네트워크.접속

	UFUNCTION(BlueprintPure, Category = "Overcooked|Network")
	EOCPlayMode GetPlayMode() const { return PlayMode; } //네트워크.상태

	UFUNCTION(BlueprintPure, Category = "Overcooked|Network")
	int32 GetMaximumNetworkPlayers() const { return MaximumNetworkPlayers; } //네트워크.인원

	UPROPERTY(BlueprintAssignable, Category = "Overcooked|Events")
	FOCPlayModeChangedSignature OnPlayModeChanged;

private:
	void SetPlayMode(EOCPlayMode NewPlayMode);

	UPROPERTY(Transient)
	EOCPlayMode PlayMode = EOCPlayMode::ListenServerHost;

	UPROPERTY(Transient)
	int32 MaximumNetworkPlayers = 4;
};
