#include "OCMenuWidget.h"
#include "OCMenuPlayerController.h"
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
		// 2P 접속 → Panda 이미지
		if (Player2PandaTexture)
		{
			Player2Image->SetBrushFromTexture(
				Player2PandaTexture,
				true
			);
		}

		// Panda 이름 표시
		Player2NameText->SetVisibility(ESlateVisibility::Visible);
		Player2NameText->SetText(FText::FromString(TEXT("Panda")));
	}
	else
	{
		// 2P 미접속 → 참가 대기
		if (Player2WaitingTexture)
		{
			Player2Image->SetBrushFromTexture(
				Player2WaitingTexture,
				true
			);
		}

		Player2NameText->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}
}
void UOCMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AOCMenuPlayerController* MenuPC =
		Cast<AOCMenuPlayerController>(GetOwningPlayer()))
	{
		MenuPC->ServerRequestPlayerSlots();
	}

	// 기존 버튼 바인딩 코드들은 그대로
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
