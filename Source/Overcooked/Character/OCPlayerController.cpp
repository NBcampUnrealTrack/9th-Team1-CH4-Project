#include "OCPlayerController.h"

#include "../Camera/OCSharedCameraActor.h"
#include "../Core/OCGameMode.h"
#include "../Core/OCGameState.h"
#include "../UI/OCHUDWidget.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"
#include "../UI/OCResultWidget.h"
#include "../UI/OCTutorialWidget.h"
#include "Components/AudioComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AOCPlayerController::AOCPlayerController()
{
	bShowMouseCursor = false;
	bAutoManageActiveCameraTarget = false;
}

void AOCPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && GameBGM)
	{
		GameBGMComponent = UGameplayStatics::SpawnSound2D(this, GameBGM);

		if (GameBGMComponent)
		{
			GameBGMComponent->SetPitchMultiplier(1.0f);
		}
	}
	
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	SetShowMouseCursor(false);
	EnsureDefaultOrderCardTextures();
	
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UOCHUDWidget>(this, HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
	if (TutorialWidgetClass)
	{
		TutorialWidget = CreateWidget<UOCTutorialWidget>(
			this,
			TutorialWidgetClass
		);

		if (TutorialWidget)
		{
			TutorialWidget->AddToViewport(100);
		}
	}
	BindGameState();

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

void AOCPlayerController::UpdateBGMSpeed(float RemainingTime)
{
	if (!IsLocalController() || !GameBGMComponent)
	{
		return;
	}

	if (RemainingTime <= 30.0f && RemainingTime > 0.0f)
	{
		if (!bBGMSpedUp)
		{
			GameBGMComponent->SetPitchMultiplier(1.3f);
			bBGMSpedUp = true;
		}
	}
	else if (RemainingTime > 30.0f)
	{
		if (bBGMSpedUp)
		{
			GameBGMComponent->SetPitchMultiplier(1.0f);
			bBGMSpedUp = false;
		}
	}
}

void AOCPlayerController::BindGameState()
{
	if (AOCGameState* GameState = GetWorld()->GetGameState<AOCGameState>())
	{
		GetWorldTimerManager().ClearTimer(GameStateRetryTimerHandle);
		GameState->OnScoreChanged.AddDynamic(
			this,
			&AOCPlayerController::HandleScoreChanged
		);

		HandleScoreChanged(GameState->GetCurrentScore(), 0);
		
		GameState->OnRemainingTimeChanged.AddDynamic(
	this,
	&AOCPlayerController::HandleRemainingTimeChanged
);

		
		
		HandleRemainingTimeChanged(GameState->GetRemainingTime());
		
		GameState->OnComboChanged.AddDynamic(
	this,
	&AOCPlayerController::HandleComboChanged
);

		HandleComboChanged(
			GameState->GetComboCount(),
			GameState->GetTipMultiplier()
		);
		
		GameState->OnOrdersChanged.AddDynamic(
	this,
	&AOCPlayerController::HandleOrdersChanged
);
		GameState->OnMatchResultReady.AddDynamic(
			this,
			&AOCPlayerController::HandleMatchResultReady
		);
		HandleOrdersChanged();
		
		GameState->OnMatchPhaseChanged.AddDynamic(
	this,
	&AOCPlayerController::HandleMatchPhaseChanged
);
		// Replication may have arrived before the local controller subscribed.
		HandleMatchPhaseChanged(GameState->GetMatchPhase(), GameState->GetMatchPhase());
	}
	else
	{
		GetWorldTimerManager().SetTimer(GameStateRetryTimerHandle, this,
			&AOCPlayerController::BindGameState, 0.1f, false);
	}
}

void AOCPlayerController::StartGame()
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

	World->ServerTravel(TEXT("/Game/Maps/DevMap"));
}

void AOCPlayerController::ServerFinishTutorial_Implementation()
{
	AOCGameMode* GameMode = GetWorld()
		? GetWorld()->GetAuthGameMode<AOCGameMode>()
		: nullptr;

	if (GameMode)
	{
		GameMode->StartRound();
	}
}

void AOCPlayerController::HandleMatchResultReady(
    const FOCMatchResult& MatchResult
)
{
    if (!IsLocalPlayerController() || !ResultWidgetClass || bResultPresentationStarted)
    {
        return;
    }
    bResultPresentationStarted = true;

    // =========================
    // TIME OVER 팝업
    // =========================
    if (TimeOverWidgetClass)
    {
        if (!TimeOverWidget)
        {
            TimeOverWidget = CreateWidget<UUserWidget>(
                this,
                TimeOverWidgetClass
            );
        }

        if (TimeOverWidget)
        {
            TimeOverWidget->AddToViewport(200);
        }
    }

    // 결과창에 필요한 값은 복사해서 타이머에 넘김
    const FOCMatchResult SavedResult = MatchResult;

    GetWorldTimerManager().SetTimer(
        ResultDelayTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this, SavedResult]()
        {
            // TimeOver 제거
            if (TimeOverWidget)
            {
                TimeOverWidget->RemoveFromParent();
            }

            // =========================
            // 기존 RESULT 처리
            // =========================
            if (!ResultWidget)
            {
                ResultWidget = CreateWidget<UOCResultWidget>(
                    this,
                    ResultWidgetClass
                );
            }

            if (!ResultWidget)
            {
                return;
            }

            // Phase and result notifications can arrive in either order.
            const AOCGameState* GameState =
                GetWorld()->GetGameState<AOCGameState>();
            const FOCMatchResult LatestResult = GameState
                ? GameState->GetMatchResult() : SavedResult;

            ResultWidget->SetResultValues(
                0,
                0,
                0,
                LatestResult.FinalScore
            );

            ResultWidget->SetStarThresholds(
                LatestResult.FinalScore,
                300,
                600,
                900
            );

            if (GameState)
            {
                ResultWidget->SetPlayerCount(
                    GameState->GetParticipatingPlayerCount()
                );
            }

            ResultWidget->SetPlayer1Name(TEXT("Rabbit"));

            if (GameState &&
                GameState->GetParticipatingPlayerCount() >= 2)
            {
                ResultWidget->SetPlayer2Name(TEXT("Panda"));
            }

            ResultWidget->AddToViewport(100);

            FInputModeUIOnly InputMode;
            InputMode.SetWidgetToFocus(
                ResultWidget->TakeWidget()
            );

            SetInputMode(InputMode);
            SetShowMouseCursor(true);

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Result Ready - Score: %d / Stars: %d"),
                LatestResult.FinalScore,
                LatestResult.EarnedStars
            );
        }),
        2.0f, // TimeOver 표시 시간
        false
    );
}

void AOCPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SharedCameraRetryTimerHandle);
	GetWorldTimerManager().ClearTimer(GameStateRetryTimerHandle);
	GetWorldTimerManager().ClearTimer(ResultDelayTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AOCPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!IsLocalPlayerController())
	{
		return;
	}

	ApplyMovementInput(ReadMovementInput());

	if (WasInputKeyJustPressed(EKeys::Tab))
	{
		ServerSwitchSinglePlayerCharacter();
	}

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

void AOCPlayerController::ConfigureSinglePlayerCharacters(
	APawn* PrimaryCharacter,
	APawn* SecondaryCharacter)
{
	if (!HasAuthority())
	{
		return;
	}

	SinglePlayerPrimaryCharacter = PrimaryCharacter;
	SinglePlayerSecondaryCharacter = SecondaryCharacter;
}

void AOCPlayerController::ServerSwitchSinglePlayerCharacter_Implementation()
{
	if (!IsValid(SinglePlayerPrimaryCharacter) || !IsValid(SinglePlayerSecondaryCharacter))
	{
		return;
	}

	APawn* NextCharacter = GetPawn() == SinglePlayerPrimaryCharacter
		? SinglePlayerSecondaryCharacter
		: SinglePlayerPrimaryCharacter;
	Possess(NextCharacter);
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
void AOCPlayerController::HandleScoreChanged(
	int32 NewScore,
	int32 ScoreDelta)
{
	if (HUDWidget)
	{
		HUDWidget->SetScore(NewScore);
	}
}
void AOCPlayerController::HandleRemainingTimeChanged(float NewRemainingTime)
{
	if (!HUDWidget)
	{
		return;
	}

	const AOCGameState* GameState =
		GetWorld()->GetGameState<AOCGameState>();

	if (!GameState)
	{
		return;
	}

	if (GameState->GetMatchPhase() == EOCMatchPhase::Countdown)
	{
		HUDWidget->SetCountdown(
			FMath::CeilToInt(NewRemainingTime)
		);

		return;
	}

	if (GameState->GetMatchPhase() == EOCMatchPhase::Playing)
	{
		HUDWidget->SetTimer(NewRemainingTime, 120.0f);
	}
}
void AOCPlayerController::HandleComboChanged(
	int32 NewComboCount,
	int32 NewTipMultiplier)
{
	if (HUDWidget)
	{
		HUDWidget->SetTipMultiplier(
			static_cast<float>(NewTipMultiplier)
		);
	}
}
void AOCPlayerController::HandleOrdersChanged()
{
	if (!HUDWidget)
	{
		return;
	}

	const AOCGameState* GameState =
		GetWorld()->GetGameState<AOCGameState>();

	if (!GameState)
	{
		return;
	}

	HUDWidget->ClearOrders();

	const TArray<FOCActiveOrder>& Orders =
		GameState->GetActiveOrdersRef();

	int32 DisplayedOrderCount = 0;

	for (const FOCActiveOrder& Order : Orders)
	{
		if (DisplayedOrderCount >= 5)
		{
			break;
		}

		if (const TObjectPtr<UTexture2D>* FoundTexture =
			OrderCardTextures.Find(Order.Recipe))
		{
			if (*FoundTexture)
			{
				HUDWidget->AddOrder(*FoundTexture);
				++DisplayedOrderCount;
			}
		}
	}
}

void AOCPlayerController::EnsureDefaultOrderCardTextures()
{
	const auto AddMissingTexture = [this](const EOCRecipeType Recipe, const TCHAR* AssetPath)
	{
		if (!OrderCardTextures.Contains(Recipe) || !OrderCardTextures[Recipe])
		{
			if (UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, AssetPath))
			{
				OrderCardTextures.Add(Recipe, Texture);
			}
		}
	};

	AddMissingTexture(
		EOCRecipeType::LettuceSalad,
		TEXT("/Game/Overcooked/UI/Images/UHD/Order/T_OC_Order_Lettuce.T_OC_Order_Lettuce"));
	AddMissingTexture(
		EOCRecipeType::TomatoSalad,
		TEXT("/Game/Overcooked/UI/Images/UHD/Order/T_OC_Order_LettuceTomato.T_OC_Order_LettuceTomato"));
	AddMissingTexture(
		EOCRecipeType::CucumberTomatoCabbageSalad,
		TEXT("/Game/Overcooked/UI/Images/UHD/Order/T_OC_Order_LettuceTomatoCucumber.T_OC_Order_LettuceTomatoCucumber"));
	AddMissingTexture(
		EOCRecipeType::ShrimpSushi,
		TEXT("/Game/Overcooked/UI/Images/UHD/Order/T_OC_Order_ShrimpSushi.T_OC_Order_ShrimpSushi"));
	AddMissingTexture(
		EOCRecipeType::OctopusSushi,
		TEXT("/Game/Overcooked/UI/Images/UHD/Order/T_OC_Order_OctopusSushi.T_OC_Order_OctopusSushi"));
	AddMissingTexture(
		EOCRecipeType::SalmonSushi,
		TEXT("/Game/Overcooked/UI/Images/UHD/Order/T_OC_Order_SalmonSushi.T_OC_Order_SalmonSushi"));
}

void AOCPlayerController::HandleMatchPhaseChanged(
	EOCMatchPhase NewPhase,
	EOCMatchPhase PreviousPhase
)
{
	// A zero-score result equals its default value and need not trigger OnRep_MatchResult.
	if (NewPhase == EOCMatchPhase::Results)
	{
		if (const AOCGameState* GameState = GetWorld()->GetGameState<AOCGameState>())
		{
			HandleMatchResultReady(GameState->GetMatchResult());
		}
	}
	if (NewPhase == EOCMatchPhase::Countdown)
	{
		if (TutorialWidget)
		{
			TutorialWidget->HideTutorial();
		}
	}
	if (!HUDWidget)
	{
		return;
	}

	if (NewPhase == EOCMatchPhase::Playing)
	{
		HUDWidget->SetCountdown(0);
	}
}
void AOCPlayerController::RequestMainMenu()
{
	ServerRequestMainMenu();
}

void AOCPlayerController::RequestNextStage()
{
	ServerRequestNextStage();
}

void AOCPlayerController::RequestRetryStage()
{
	ServerRequestRetryStage();
}

void AOCPlayerController::ServerRequestRetryStage_Implementation()
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

	const FString CurrentLevel =
		UGameplayStatics::GetCurrentLevelName(this, true);

	const FString TravelURL = FString::Printf(
		TEXT("/Game/Overcooked/Maps/%s?ExpectedPlayers=%d"),
		*CurrentLevel,
		FMath::Max(World->GetNumPlayerControllers(), 1)
	);

	World->ServerTravel(TravelURL);
}
void AOCPlayerController::ServerRequestMainMenu_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TEXT("/Game/Overcooked/Maps/MenuMap"));
	}
}

void AOCPlayerController::ServerRequestNextStage_Implementation()
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

	const FString CurrentLevel =
		UGameplayStatics::GetCurrentLevelName(this, true);

	FString NextLevel;

	if (CurrentLevel.Equals(TEXT("tutorial"), ESearchCase::IgnoreCase))
	{
		NextLevel = TEXT("/Game/Overcooked/Maps/Leveone");
	}
	else if (CurrentLevel.Equals(TEXT("Leveone"), ESearchCase::IgnoreCase))
	{
		NextLevel = TEXT("/Game/Overcooked/Maps/Levetwo");
	}
	else if (CurrentLevel.Equals(TEXT("Levetwo"), ESearchCase::IgnoreCase))
	{
		NextLevel = TEXT("/Game/Overcooked/Maps/Levethree");
	}

	if (!NextLevel.IsEmpty())
	{
		World->ServerTravel(NextLevel);
	}
}