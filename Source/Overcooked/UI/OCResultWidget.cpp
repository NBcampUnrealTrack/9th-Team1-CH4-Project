#include "OCResultWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

void UOCResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RetryButton)
	{
		RetryButton->OnClicked.AddDynamic(
			this,
			&UOCResultWidget::OnRetryClicked
		);
	}

	if (NextButton)
	{
		NextButton->OnClicked.AddDynamic(
			this,
			&UOCResultWidget::OnNextClicked
		);
	}
}

void UOCResultWidget::SetStageText(const FString& StageName)
{
	if (StageText)
	{
		StageText->SetText(FText::FromString(StageName));
	}
}

void UOCResultWidget::SetResultValues(
	int32 DeliveredValue,
	int32 TipValue,
	int32 FailedValue,
	int32 TotalScore)
{
	if (DeliveredValueText)
	{
		DeliveredValueText->SetText(
			FText::AsNumber(DeliveredValue)
		);
	}

	if (TipValueText)
	{
		TipValueText->SetText(
			FText::AsNumber(TipValue)
		);
	}

	if (FailedValueText)
	{
		FailedValueText->SetText(
			FText::AsNumber(FailedValue)
		);
	}

	if (TotalScoreText)
	{
		TotalScoreText->SetText(
			FText::AsNumber(TotalScore)
		);
	}
}

void UOCResultWidget::SetStarThresholds(
	int32 CurrentScore,
	int32 OneStarScore,
	int32 TwoStarScore,
	int32 ThreeStarScore)
{
	if (Star1ScoreText)
	{
		Star1ScoreText->SetText(FText::AsNumber(OneStarScore));
	}

	if (Star2ScoreText)
	{
		Star2ScoreText->SetText(FText::AsNumber(TwoStarScore));
	}

	if (Star3ScoreText)
	{
		Star3ScoreText->SetText(FText::AsNumber(ThreeStarScore));
	}

	if (Star1Image)
	{
		Star1Image->SetVisibility(
			CurrentScore >= OneStarScore
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden
		);
	}

	if (Star2Image)
	{
		Star2Image->SetVisibility(
			CurrentScore >= TwoStarScore
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden
		);
	}

	if (Star3Image)
	{
		Star3Image->SetVisibility(
			CurrentScore >= ThreeStarScore
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden
		);
	}
}

void UOCResultWidget::SetPlayerCount(int32 PlayerCount)
{
	if (Player1Panel)
	{
		Player1Panel->SetVisibility(
			PlayerCount >= 1
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed
		);
	}

	if (Player2Panel)
	{
		Player2Panel->SetVisibility(
			PlayerCount >= 2
			? ESlateVisibility::Visible
			: ESlateVisibility::Collapsed
		);
	}
}

void UOCResultWidget::SetPlayer1Name(const FString& PlayerName)
{
	if (Player1NameText)
	{
		Player1NameText->SetText(
			FText::FromString(PlayerName)
		);
	}
}

void UOCResultWidget::SetPlayer2Name(const FString& PlayerName)
{
	if (Player2NameText)
	{
		Player2NameText->SetText(
			FText::FromString(PlayerName)
		);
	}
}

void UOCResultWidget::OnRetryClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Retry Button Clicked"));

	BP_OnRetryClicked();
}

void UOCResultWidget::OnNextClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Next Button Clicked"));

	BP_OnNextClicked();
}
