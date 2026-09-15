#include "OCMenuPlayerController.h"
#include "OCMenuWidget.h"
#include "../UI/OCMenuPlayerController.h"
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

void AOCMenuPlayerController::ClientUpdatePlayerSlots_Implementation(
	int32 PlayerCount)
{
	if (UOCMenuWidget* MenuWidget = FindMenuWidget())
	{
		MenuWidget->SetPlayer2Connected(PlayerCount >= 2);
	}
}
