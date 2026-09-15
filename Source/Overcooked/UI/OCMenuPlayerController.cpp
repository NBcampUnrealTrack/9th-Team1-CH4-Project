#include "OCMenuPlayerController.h"
#include "OCMenuWidget.h"
#include "OCConnectionWidget.h"
#include "OCSessionManager.h"
#include "../UI/OCMenuPlayerController.h"
#include "../Core/OCMenuGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

AOCMenuPlayerController::AOCMenuPlayerController()
{
	bShowMouseCursor = true;
}

UOCMenuWidget* AOCMenuPlayerController::FindMenuWidget() const
{
	TArray<UUserWidget*> FoundWidgets;

	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
		GetWorld(),
		FoundWidgets,
		UOCMenuWidget::StaticClass(),
		false
	);

	for (UUserWidget* Widget : FoundWidgets)
	{
		if (UOCMenuWidget* MenuWidget = Cast<UOCMenuWidget>(Widget))
		{
			return MenuWidget;
		}
	}

	return nullptr;
}

void AOCMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		SessionManager = NewObject<UOCSessionManager>(this);

		if (SessionManager)
		{
			SessionManager->Initialize();
		}
	}
}
void AOCMenuPlayerController::HostGame()
{
	if (SessionManager)
	{
		SessionManager->HostSession();
	}
}
void AOCMenuPlayerController::JoinGame()
{
	if (SessionManager)
	{
		SessionManager->FindAndJoinSession();
	}
}

void AOCMenuPlayerController::JoinGameByAddress(const FString& Address)
{
	FString ConnectAddress = Address;
	ConnectAddress.TrimStartAndEndInline();
	ConnectAddress.RemoveFromStart(TEXT("open "), ESearchCase::IgnoreCase);

	if (ConnectAddress.IsEmpty())
	{
		ConnectAddress = TEXT("127.0.0.1");
	}

	ClientTravel(ConnectAddress, TRAVEL_Absolute);
}

void AOCMenuPlayerController::ShowConnectionChoice(UOCMenuWidget* SourceMenu)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!ConnectionWidget)
	{
		ConnectionWidget = CreateWidget<UOCConnectionWidget>(this, UOCConnectionWidget::StaticClass());
	}

	if (ConnectionWidget)
	{
		ConnectionWidget->InitializeConnectionScreen(SourceMenu);
		ConnectionWidget->AddToViewport(100);
	}
}

void AOCMenuPlayerController::RequestStartGame()
{
	ServerRequestStartGame();
}

void AOCMenuPlayerController::ServerRequestStartGame_Implementation()
{
	if (AOCMenuGameMode* MenuGameMode = GetWorld()->GetAuthGameMode<AOCMenuGameMode>())
	{
		MenuGameMode->StartGame(this);
	}
}
void AOCMenuPlayerController::ClientUpdatePlayerSlots_Implementation(
	int32 PlayerCount)
{
	if (UOCMenuWidget* MenuWidget = FindMenuWidget())
	{
		MenuWidget->SetPlayer2Connected(PlayerCount >= 2);
	}
}
void AOCMenuPlayerController::ServerRequestPlayerSlots_Implementation()
{
	if (AOCMenuGameMode* MenuGameMode =
		GetWorld()->GetAuthGameMode<AOCMenuGameMode>())
	{
		MenuGameMode->UpdatePlayerSlots();
	}
}
