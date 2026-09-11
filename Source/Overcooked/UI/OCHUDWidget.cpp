#include "OCHUDWidget.h"

#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"

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

void UOCHUDWidget::AddOrder(UTexture2D* OrderTexture)
{
	if (!OrderBox || !OrderTexture)
	{
		return;
	}

	USizeBox* OrderSizeBox = NewObject<USizeBox>(this);
	if (!OrderSizeBox)
	{
		return;
	}

	OrderSizeBox->SetWidthOverride(190.0f);
	OrderSizeBox->SetHeightOverride(200.0f);

	UImage* OrderImage = NewObject<UImage>(this);
	if (!OrderImage)
	{
		return;
	}

	OrderImage->SetBrushFromTexture(OrderTexture, true);

	OrderSizeBox->AddChild(OrderImage);

	UHorizontalBoxSlot* OrderSlot =
		OrderBox->AddChildToHorizontalBox(OrderSizeBox);

	if (OrderSlot)
	{
		OrderSlot->SetPadding(FMargin(8.0f, 0.0f));
		OrderSlot->SetHorizontalAlignment(HAlign_Center);
		OrderSlot->SetVerticalAlignment(VAlign_Center);
	}
}

void UOCHUDWidget::ClearOrders()
{
	if (OrderBox)
	{
		OrderBox->ClearChildren();
	}
}