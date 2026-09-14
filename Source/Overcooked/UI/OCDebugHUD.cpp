#include "OCDebugHUD.h"

#include "../Core/OCGameState.h"
#include "../Core/OCRecipeLibrary.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "Engine/Engine.h"

void AOCDebugHUD::DrawHUD()
{
	Super::DrawHUD();

	const AOCGameState* State = GetWorld() ? GetWorld()->GetGameState<AOCGameState>() : nullptr;
	if (!IsValid(State) || !Canvas)
	{
		return;
	}

	const int32 RemainingSeconds = FMath::Max(0, FMath::CeilToInt(State->GetRemainingTime()));
	if (State->GetMatchPhase() == EOCMatchPhase::Countdown)
	{
		DrawCenteredText(FString::FromInt(RemainingSeconds), 80.0f, 3.0f, FLinearColor::Yellow);
	}
	else if (State->GetMatchPhase() == EOCMatchPhase::Playing)
	{
		const int32 Minutes = RemainingSeconds / 60;
		const int32 Seconds = RemainingSeconds % 60;
		const int32 CurrentScore = State->GetCurrentScore();
		if (!bHasInitializedScore)
		{
			PreviousScore = CurrentScore;
			bHasInitializedScore = true;
		}
		else if (CurrentScore != PreviousScore)
		{
			DisplayedScoreDelta = CurrentScore - PreviousScore;
			ScoreDeltaDisplayEndTime = GetWorld()->GetTimeSeconds() + 2.0f;
			PreviousScore = CurrentScore;
		}

		const bool bShowScoreDelta = DisplayedScoreDelta != 0
			&& GetWorld()->GetTimeSeconds() < ScoreDeltaDisplayEndTime;
		const FString ScoreText = bShowScoreDelta
			? FString::Printf(TEXT("점수 %d  %s%d"), CurrentScore, DisplayedScoreDelta > 0 ? TEXT("+") : TEXT(""), DisplayedScoreDelta)
			: FString::Printf(TEXT("점수 %d"), CurrentScore);
		DrawBottomLeftText(ScoreText, 30.0f, 1.0f, FLinearColor::Black);
		DrawBottomLeftText(
			FString::Printf(TEXT("콤보 %d  팁 x%d"), State->GetComboCount(), State->GetTipMultiplier()),
			70.0f,
			0.75f,
			FLinearColor::Black);
		DrawBottomRightText(FString::Printf(TEXT("시간 %02d:%02d"), Minutes, Seconds), 1.0f, FLinearColor::Black);
		DrawOrders(*State);
	}
	else
	{
		bHasInitializedScore = false;
		DisplayedScoreDelta = 0;
	}
}

void AOCDebugHUD::DrawBottomLeftText(
	const FString& Text,
	const float BottomOffset,
	const float Scale,
	const FLinearColor& Color) const
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Canvas || !Font)
	{
		return;
	}

	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	Canvas->StrLen(Font, Text, TextWidth, TextHeight);
	Canvas->SetDrawColor(Color.ToFColor(true));
	Canvas->DrawText(
		Font,
		Text,
		30.0f,
		Canvas->SizeY - TextHeight * Scale - BottomOffset,
		Scale,
		Scale);
}

void AOCDebugHUD::DrawOrders(const AOCGameState& State) const
{
	UFont* LargeFont = GEngine ? GEngine->GetLargeFont() : nullptr;
	UFont* SmallFont = GEngine ? GEngine->GetSmallFont() : nullptr;
	if (!Canvas || !LargeFont || !SmallFont)
	{
		return;
	}

	const TArray<FOCActiveOrder>& Orders = State.GetActiveOrdersRef();
	const float BlockWidth = 125.0f;
	const float BlockHeight = 92.0f;
	const float Gap = 10.0f;
	const int32 BlocksPerRow = FMath::Max(1, FMath::FloorToInt((Canvas->SizeX - Gap) / (BlockWidth + Gap)));

	for (int32 Index = 0; Index < Orders.Num(); ++Index)
	{
		const int32 Column = Index % BlocksPerRow;
		const int32 Row = Index / BlocksPerRow;
		const float X = Gap + Column * (BlockWidth + Gap);
		const float Y = 20.0f + Row * (BlockHeight + Gap);
		const FOCRecipeDefinition Recipe = UOCRecipeLibrary::GetRecipeDefinition(Orders[Index].Recipe);

		FCanvasTileItem Background(
			FVector2D(X, Y),
			FVector2D(BlockWidth, BlockHeight),
			FLinearColor(0.02f, 0.02f, 0.02f, 0.82f));
		Background.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Background);
		Canvas->SetDrawColor(FColor(255, 190, 40));
		Canvas->DrawText(
			LargeFont,
			FString::Printf(TEXT("%d. %s"), Index + 1, *Recipe.DisplayName.ToString()),
			X + 10.0f,
			Y + 12.0f,
			1.0f,
			1.0f);
		const float OrderAge = FMath::Max(0.0f, State.GetServerWorldTimeSeconds() - Orders[Index].CreatedAtServerTime);
		const int32 OrderRemainingSeconds = FMath::Max(0, FMath::CeilToInt(Orders[Index].TimeLimit - OrderAge));
		const float RemainingRatio = 1.0f - FMath::Clamp(OrderAge / FMath::Max(1.0f, Orders[Index].TimeLimit), 0.0f, 1.0f);
		const int32 CurrentTip = FMath::RoundToInt(FMath::Lerp(3.0f, 8.0f, RemainingRatio));
		const FColor OrderTimeColor = CurrentTip >= 7
			? FColor(80, 220, 80)
			: CurrentTip >= 6
				? FColor::Yellow
				: CurrentTip >= 4
					? FColor(255, 140, 30)
					: FColor::Red;
		Canvas->SetDrawColor(OrderTimeColor);
		Canvas->DrawText(
			SmallFont,
			FString::Printf(TEXT("남은 시간 %d초"), OrderRemainingSeconds),
			X + 10.0f,
			Y + 48.0f,
			0.75f,
			0.75f);
		Canvas->SetDrawColor(FColor::White);
		Canvas->DrawText(SmallFont, Recipe.IngredientSummary.ToString(), X + 10.0f, Y + 68.0f, 0.75f, 0.75f);
	}
}

void AOCDebugHUD::DrawBottomRightText(
	const FString& Text,
	const float Scale,
	const FLinearColor& Color) const
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Canvas || !Font)
	{
		return;
	}

	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	Canvas->StrLen(Font, Text, TextWidth, TextHeight);
	Canvas->SetDrawColor(Color.ToFColor(true));
	Canvas->DrawText(
		Font,
		Text,
		Canvas->SizeX - TextWidth * Scale - 30.0f,
		Canvas->SizeY - TextHeight * Scale - 30.0f,
		Scale,
		Scale);
}

void AOCDebugHUD::DrawCenteredText(
	const FString& Text,
	const float ScreenY,
	const float Scale,
	const FLinearColor& Color) const
{
	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Canvas || !Font)
	{
		return;
	}

	float TextWidth = 0.0f;
	float TextHeight = 0.0f;
	Canvas->StrLen(Font, Text, TextWidth, TextHeight);
	Canvas->SetDrawColor(Color.ToFColor(true));
	Canvas->DrawText(Font, Text, (Canvas->SizeX - TextWidth * Scale) * 0.5f, ScreenY, Scale, Scale);
}
