#include "OvercookedMenuWidget.h"
#include "Components/Button.h"

void UOvercookedMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(
			this,
			&UOvercookedMenuWidget::OnStartButtonClicked
		);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(
			this,
			&UOvercookedMenuWidget::OnSettingsButtonClicked
		);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(
			this,
			&UOvercookedMenuWidget::OnExitButtonClicked
		);
	}
}

void UOvercookedMenuWidget::OnStartButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Start Button Clicked"));
}

void UOvercookedMenuWidget::OnSettingsButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Settings Button Clicked"));
}

void UOvercookedMenuWidget::OnExitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Exit Button Clicked"));
}