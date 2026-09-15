#include "OCMenuWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UOCMenuWidget::SetPlayer2Connected(bool bConnected)
{
	if (!Player2Image || !Player2NameText)
	{
		return;
	}

	if (bConnected)
	{
		// 참가대기 → Panda
		if (Player2PandaTexture)
		{
			Player2Image->SetBrushFromTexture(
				Player2PandaTexture,
				true
			);
		}

		Player2NameText->SetText(FText::FromString(TEXT("Panda")));
		Player2NameText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		// Panda → 참가대기
		if (Player2WaitingTexture)
		{
			Player2Image->SetBrushFromTexture(
				Player2WaitingTexture,
				true
			);
		}

		Player2NameText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
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