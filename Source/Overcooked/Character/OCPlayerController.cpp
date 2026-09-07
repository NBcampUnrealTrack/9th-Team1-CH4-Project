#include "OCPlayerController.h"

#include "../Camera/OCSharedCameraActor.h"
#include "../Core/OCGameMode.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"

AOCPlayerController::AOCPlayerController()
{
	bShowMouseCursor = false;
	bAutoManageActiveCameraTarget = false;
}

void AOCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalPlayerController())
	{
		return;
	}

	if (!TryUseSharedCamera())
	{
		GetWorldTimerManager().SetTimer(
			SharedCameraRetryTimerHandle,
			this,
			&AOCPlayerController::RefreshSharedCamera,
			0.1f,
			true);
	}
}

void AOCPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!IsLocalPlayerController())
	{
		return;
	}

	ApplyMovementInput(ReadMovementInput());

	int32 DebugOrderIndex = INDEX_NONE;
	if (WasInputKeyJustPressed(EKeys::One))
	{
		DebugOrderIndex = 0;
	}
	else if (WasInputKeyJustPressed(EKeys::Two))
	{
		DebugOrderIndex = 1;
	}
	else if (WasInputKeyJustPressed(EKeys::Three))
	{
		DebugOrderIndex = 2;
	}
	else if (WasInputKeyJustPressed(EKeys::Four))
	{
		DebugOrderIndex = 3;
	}
	else if (WasInputKeyJustPressed(EKeys::Five))
	{
		DebugOrderIndex = 4;
	}
	else if (WasInputKeyJustPressed(EKeys::Six))
	{
		DebugOrderIndex = 5;
	}

	if (DebugOrderIndex != INDEX_NONE)
	{
		ServerDebugCompleteOrder(DebugOrderIndex);
	}
}

void AOCPlayerController::ServerDebugCompleteOrder_Implementation(const int32 OrderIndex)
{
	if (AOCGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AOCGameMode>() : nullptr)
	{
		GameMode->DebugCompleteOrderAtIndex(OrderIndex);
	}
}

void AOCPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RefreshSharedCamera();
}

void AOCPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	RefreshSharedCamera();
}

bool AOCPlayerController::TryUseSharedCamera()
{
	if (!IsLocalPlayerController())
	{
		return false;
	}

	for (TActorIterator<AOCSharedCameraActor> It(GetWorld()); It; ++It)
	{
		SetViewTarget(*It);
		GetWorldTimerManager().ClearTimer(SharedCameraRetryTimerHandle);
		return true;
	}

	return false;
}

void AOCPlayerController::RefreshSharedCamera()
{
	TryUseSharedCamera();
}

FVector2D AOCPlayerController::ReadMovementInput() const
{
	FVector2D Input(
		GetInputAnalogKeyState(EKeys::Gamepad_LeftX),
		GetInputAnalogKeyState(EKeys::Gamepad_LeftY));

	if (GetLocalPlayer() && GetLocalPlayer()->GetControllerId() == 0)
	{
		Input.X +=
			(IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right) ? 1.0f : 0.0f) -
			(IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left) ? 1.0f : 0.0f);
		Input.Y +=
			(IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up) ? 1.0f : 0.0f) -
			(IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down) ? 1.0f : 0.0f);
	}

	if (Input.SizeSquared() < FMath::Square(GamepadDeadZone))
	{
		return FVector2D::ZeroVector;
	}

	return Input.GetClampedToMaxSize(1.0f);
}

void AOCPlayerController::ApplyMovementInput(const FVector2D& MovementInput)
{
	APawn* ControlledPawn = GetPawn();
	if (!IsValid(ControlledPawn) || MovementInput.IsNearlyZero())
	{
		return;
	}

	const FRotator CameraRotation = PlayerCameraManager
		? PlayerCameraManager->GetCameraRotation()
		: GetControlRotation();
	const FRotator YawRotation(0.0f, CameraRotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardDirection, MovementInput.Y);
	ControlledPawn->AddMovementInput(RightDirection, MovementInput.X);
}
