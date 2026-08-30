#include "OCPlayerState.h"

#include "Net/UnrealNetwork.h"

AOCPlayerState::AOCPlayerState()
{
	bReplicates = true;
}

void AOCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOCPlayerState, PlayerSlotIndex);
}

void AOCPlayerState::SetPlayerSlotIndex(const int32 NewSlotIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	const int32 ClampedSlotIndex = FMath::Clamp(NewSlotIndex, 0, 3);
	if (PlayerSlotIndex == ClampedSlotIndex)
	{
		return;
	}

	const int32 PreviousSlotIndex = PlayerSlotIndex;
	PlayerSlotIndex = ClampedSlotIndex;
	OnPlayerSlotChanged.Broadcast(PlayerSlotIndex, PreviousSlotIndex);
	ForceNetUpdate();
}

void AOCPlayerState::OnRep_PlayerSlotIndex(const int32 PreviousSlotIndex)
{
	OnPlayerSlotChanged.Broadcast(PlayerSlotIndex, PreviousSlotIndex);
}
