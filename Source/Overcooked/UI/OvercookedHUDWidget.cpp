#include "OvercookedHUDWidget.h"
#include "Components/TextBlock.h"

void UOvercookedHUDWidget::SetScore(int32 NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::AsNumber(NewScore));
	}
}

void UOvercookedHUDWidget::SetRemainingTime(float NewTime)
{
	if (TimerText)
	{
		const int32 TotalSeconds = FMath::Max(0, FMath::CeilToInt(NewTime));
		const int32 Minutes = TotalSeconds / 60;
		const int32 Seconds = TotalSeconds % 60;

		TimerText->SetText(
			FText::FromString(
				FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)
			)
		);
	}
}