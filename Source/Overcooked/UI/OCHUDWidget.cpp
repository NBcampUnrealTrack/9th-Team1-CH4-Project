#include "OCHUDWidget.h"

#include "Components/TextBlock.h"

void UOCHUDWidget::SetScore(int32 NewScore)
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::AsNumber(NewScore));
	}
}

void UOCHUDWidget::SetRemainingTime(float NewTime)
{
	if (TimerText)
	{
		const int32 TotalSeconds = FMath::Max(0, FMath::CeilToInt(NewTime));

		const int32 Minutes = TotalSeconds / 60;
		const int32 Seconds = TotalSeconds % 60;

		const FString TimeString =
			FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

		TimerText->SetText(FText::FromString(TimeString));
	}
}