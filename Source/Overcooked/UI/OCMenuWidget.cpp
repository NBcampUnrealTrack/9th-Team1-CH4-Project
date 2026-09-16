#include "OCMenuWidget.h"
#include "OCMenuPlayerController.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OCSettingsWidget.h"
#include "Kismet/KismetSystemLibrary.h"

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
		StartButton->OnClicked.Clear();
		StartButton->OnClicked.AddDynamic(
			this,
			&UOCMenuWidget::OnStartButtonClicked
		);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.Clear();
		SettingsButton->OnClicked.AddDynamic(
			this,
			&UOCMenuWidget::OnSettingsButtonClicked
		);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.Clear();
		ExitButton->OnClicked.AddDynamic(
			this,
			&UOCMenuWidget::OnExitButtonClicked
		);
	}

	if (HostButton)
	{
		HostButton->OnClicked.Clear();
		HostButton->OnClicked.AddDynamic(this, &UOCMenuWidget::OnConnectionButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.Clear();
		JoinButton->OnClicked.AddDynamic(this, &UOCMenuWidget::OnConnectionButtonClicked);
	}
	
}

void UOCMenuWidget::OnStartButtonClicked()
{
	if (AOCMenuPlayerController* MenuPC = Cast<AOCMenuPlayerController>(GetOwningPlayer()))
	{
		MenuPC->RequestStartGame();
	}
}

void UOCMenuWidget::OnConnectionButtonClicked()
{
	if (AOCMenuPlayerController* MenuPC = Cast<AOCMenuPlayerController>(GetOwningPlayer()))
	{
		SetVisibility(ESlateVisibility::Collapsed);
		MenuPC->ShowConnectionChoice(this);
	}
}

void UOCMenuWidget::OnSettingsButtonClicked()
{
	APlayerController* OwningPlayer = GetOwningPlayer();

	if (!OwningPlayer || !SettingsWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SettingsWidgetClass is not assigned."));
		return;
	}

	UOCSettingsWidget* SettingsWidget =
		CreateWidget<UOCSettingsWidget>(
			OwningPlayer,
			SettingsWidgetClass
		);

	if (SettingsWidget)
	{
		SettingsWidget->AddToViewport();
	}
}

void UOCMenuWidget::OnExitButtonClicked()
{
	APlayerController* OwningPlayer = GetOwningPlayer();

	if (!OwningPlayer)
	{
		return;
	}

	UKismetSystemLibrary::QuitGame(
		this,
		OwningPlayer,
		EQuitPreference::Quit,
		false
	);
}
