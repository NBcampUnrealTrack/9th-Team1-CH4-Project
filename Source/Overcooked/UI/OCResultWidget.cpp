#include "OCResultWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

UOCResultWidget::UOCResultWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UOCResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RetryButton)
	{
		RetryButton->OnClicked.AddUniqueDynamic(
			this,
			&UOCResultWidget::OnRetryClicked
		);
	}

	if (NextButton)
	{
		NextButton->OnClicked.AddUniqueDynamic(
			this,
			&UOCResultWidget::OnNextClicked
		);
	}

	SetKeyboardFocus();
}

void UOCResultWidget::NativeDestruct()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			ScoreCountTimer
		);
	}

	Super::NativeDestruct();
}

void UOCResultWidget::SetStageText(
	const FString& StageName)
{
	if (StageText)
	{
		StageText->SetText(
			FText::FromString(StageName)
		);
	}
}

void UOCResultWidget::SetResultValues(
	int32 DeliveredValue,
	int32 TipValue,
	int32 FailedValue,
	int32 TotalScore)
{
	TargetDeliveredValue = DeliveredValue;
	TargetTipValue = TipValue;
	TargetScore = TotalScore;

	// 카운트업 대상은 0부터 시작
	if (DeliveredValueText)
	{
		DeliveredValueText->SetText(
			FText::AsNumber(0)
		);
	}

	if (TipValueText)
	{
		TipValueText->SetText(
			FText::AsNumber(0)
		);
	}

	// 실패 점수는 현재 바로 표시
	if (FailedValueText)
	{
		FailedValueText->SetText(
			FText::AsNumber(FailedValue)
		);
	}

	if (TotalScoreText)
	{
		TotalScoreText->SetText(
			FText::AsNumber(0)
		);
	}
}

void UOCResultWidget::SetStarThresholds(
	int32 CurrentScore,
	int32 OneStarScore,
	int32 TwoStarScore,
	int32 ThreeStarScore)
{
	TargetScore = CurrentScore;

	OneStarThreshold = OneStarScore;
	TwoStarThreshold = TwoStarScore;
	ThreeStarThreshold = ThreeStarScore;

	if (Star1ScoreText)
	{
		Star1ScoreText->SetText(
			FText::AsNumber(OneStarScore)
		);
	}

	if (Star2ScoreText)
	{
		Star2ScoreText->SetText(
			FText::AsNumber(TwoStarScore)
		);
	}

	if (Star3ScoreText)
	{
		Star3ScoreText->SetText(
			FText::AsNumber(ThreeStarScore)
		);
	}

	if (Star1Image)
	{
		Star1Image->SetVisibility(
			ESlateVisibility::Hidden
		);

		Star1Image->SetRenderScale(
			FVector2D(1.0f, 1.0f)
		);
	}

	if (Star2Image)
	{
		Star2Image->SetVisibility(
			ESlateVisibility::Hidden
		);

		Star2Image->SetRenderScale(
			FVector2D(1.0f, 1.0f)
		);
	}

	if (Star3Image)
	{
		Star3Image->SetVisibility(
			ESlateVisibility::Hidden
		);

		Star3Image->SetRenderScale(
			FVector2D(1.0f, 1.0f)
		);
	}

	bStar1Unlocked = false;
	bStar2Unlocked = false;
	bStar3Unlocked = false;

	StartScoreCountUp();
}

void UOCResultWidget::StartScoreCountUp()
{
	if (!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(
		ScoreCountTimer
	);

	DisplayedScore = 0;
	bIsCountingScore = true;

	ScoreCountStartTime =
		GetWorld()->GetTimeSeconds();

	if (DeliveredValueText)
	{
		DeliveredValueText->SetText(
			FText::AsNumber(0)
		);
	}

	if (TipValueText)
	{
		TipValueText->SetText(
			FText::AsNumber(0)
		);
	}

	if (TotalScoreText)
	{
		TotalScoreText->SetText(
			FText::AsNumber(0)
		);
	}

	GetWorld()->GetTimerManager().SetTimer(
		ScoreCountTimer,
		this,
		&UOCResultWidget::UpdateScoreCount,
		0.016f,
		true
	);
}

void UOCResultWidget::UpdateScoreCount()
{
	if (!GetWorld())
	{
		return;
	}

	const float ElapsedTime =
		GetWorld()->GetTimeSeconds()
		- ScoreCountStartTime;

	const float Alpha =
		FMath::Clamp(
			ElapsedTime / ScoreCountDuration,
			0.0f,
			1.0f
		);

	// 배달 점수
	const int32 DisplayedDelivered =
		FMath::RoundToInt(
			FMath::Lerp(
				0.0f,
				static_cast<float>(
					TargetDeliveredValue
				),
				Alpha
			)
		);

	// 팁 점수
	const int32 DisplayedTip =
		FMath::RoundToInt(
			FMath::Lerp(
				0.0f,
				static_cast<float>(
					TargetTipValue
				),
				Alpha
			)
		);

	// 총점
	DisplayedScore =
		FMath::RoundToInt(
			FMath::Lerp(
				0.0f,
				static_cast<float>(
					TargetScore
				),
				Alpha
			)
		);

	if (DeliveredValueText)
	{
		DeliveredValueText->SetText(
			FText::AsNumber(
				DisplayedDelivered
			)
		);
	}

	if (TipValueText)
	{
		TipValueText->SetText(
			FText::AsNumber(
				DisplayedTip
			)
		);
	}

	if (TotalScoreText)
	{
		TotalScoreText->SetText(
			FText::AsNumber(
				DisplayedScore
			)
		);
	}

	// 현재 총점에 따라 별 해금
	CheckStarUnlocks();

	// 3초 완료
	if (Alpha >= 1.0f)
	{
		DisplayedScore = TargetScore;

		if (DeliveredValueText)
		{
			DeliveredValueText->SetText(
				FText::AsNumber(
					TargetDeliveredValue
				)
			);
		}

		if (TipValueText)
		{
			TipValueText->SetText(
				FText::AsNumber(
					TargetTipValue
				)
			);
		}

		if (TotalScoreText)
		{
			TotalScoreText->SetText(
				FText::AsNumber(
					TargetScore
				)
			);
		}

		bIsCountingScore = false;

		GetWorld()->GetTimerManager().ClearTimer(
			ScoreCountTimer
		);
	}
}

void UOCResultWidget::CheckStarUnlocks()
{
	if (!bStar1Unlocked &&
		DisplayedScore >= OneStarThreshold)
	{
		bStar1Unlocked = true;

		if (Star1Image)
		{
			Star1Image->SetVisibility(
				ESlateVisibility::Visible
			);
		}

		if (Star1Pop)
		{
			PlayAnimation(
				Star1Pop
			);
		}
	}

	if (!bStar2Unlocked &&
		DisplayedScore >= TwoStarThreshold)
	{
		bStar2Unlocked = true;

		if (Star2Image)
		{
			Star2Image->SetVisibility(
				ESlateVisibility::Visible
			);
		}

		if (Star2Pop)
		{
			PlayAnimation(
				Star2Pop
			);
		}
	}

	if (!bStar3Unlocked &&
		DisplayedScore >= ThreeStarThreshold)
	{
		bStar3Unlocked = true;

		if (Star3Image)
		{
			Star3Image->SetVisibility(
				ESlateVisibility::Visible
			);
		}

		if (Star3Pop)
		{
			PlayAnimation(
				Star3Pop
			);
		}
	}
}

void UOCResultWidget::SkipScoreCountUp()
{
	if (!bIsCountingScore)
	{
		return;
	}

	bIsCountingScore = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			ScoreCountTimer
		);
	}

	// 재생 중인 별 팝 애니메이션 종료
	StopAllAnimations();

	DisplayedScore = TargetScore;

	// 모든 숫자를 즉시 최종값으로
	if (DeliveredValueText)
	{
		DeliveredValueText->SetText(
			FText::AsNumber(
				TargetDeliveredValue
			)
		);
	}

	if (TipValueText)
	{
		TipValueText->SetText(
			FText::AsNumber(
				TargetTipValue
			)
		);
	}

	if (TotalScoreText)
	{
		TotalScoreText->SetText(
			FText::AsNumber(
				TargetScore
			)
		);
	}

	// 최종 점수 기준으로 획득 별 결정
	bStar1Unlocked =
		TargetScore >= OneStarThreshold;

	bStar2Unlocked =
		TargetScore >= TwoStarThreshold;

	bStar3Unlocked =
		TargetScore >= ThreeStarThreshold;

	if (Star1Image)
	{
		Star1Image->SetVisibility(
			bStar1Unlocked
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden
		);

		Star1Image->SetRenderScale(
			FVector2D(1.0f, 1.0f)
		);
	}

	if (Star2Image)
	{
		Star2Image->SetVisibility(
			bStar2Unlocked
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden
		);

		Star2Image->SetRenderScale(
			FVector2D(1.0f, 1.0f)
		);
	}

	if (Star3Image)
	{
		Star3Image->SetVisibility(
			bStar3Unlocked
			? ESlateVisibility::Visible
			: ESlateVisibility::Hidden
		);

		Star3Image->SetRenderScale(
			FVector2D(1.0f, 1.0f)
		);
	}
}

FReply UOCResultWidget::NativeOnPreviewKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (bIsCountingScore)
	{
		SkipScoreCountUp();

		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(
		InGeometry,
		InKeyEvent
	);
}

void UOCResultWidget::SetPlayerCount(
	int32 PlayerCount)
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

void UOCResultWidget::SetPlayer1Name(
	const FString& PlayerName)
{
	if (Player1NameText)
	{
		Player1NameText->SetText(
			FText::FromString(
				PlayerName
			)
		);
	}
}

void UOCResultWidget::SetPlayer2Name(
	const FString& PlayerName)
{
	if (Player2NameText)
	{
		Player2NameText->SetText(
			FText::FromString(
				PlayerName
			)
		);
	}
}

void UOCResultWidget::OnRetryClicked()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Retry Button Clicked")
	);

	BP_OnRetryClicked();
}

void UOCResultWidget::OnNextClicked()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Next Button Clicked")
	);

	BP_OnNextClicked();
}