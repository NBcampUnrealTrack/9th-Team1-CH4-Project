#include "OCMenuWidget.h"
#include "Components/Button.h"

void UOCMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(
			this,
			&UOCMenuWidget::OnStartButtonClicked
		);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(
			this,
			&UOCMenuWidget::OnSettingsButtonClicked
		);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(
			this,
			&UOCMenuWidget::OnExitButtonClicked
		);
	}
}

void UOCMenuWidget::OnStartButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Start Button Clicked"));
}

void UOCMenuWidget::OnSettingsButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Settings Button Clicked"));
}

void UOCMenuWidget::OnExitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Exit Button Clicked"));
}