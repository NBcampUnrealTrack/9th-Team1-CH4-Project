#include "OCStoryWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

UOCStoryWidget::UOCStoryWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UOCStoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bCanContinue = false;

	if (ContinueButton)
	{
		ContinueButton->SetVisibility(ESlateVisibility::Hidden);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	SetKeyboardFocus();

	GetWorld()->GetTimerManager().SetTimer(
		ContinueTimerHandle,
		this,
		&UOCStoryWidget::EnableContinue,
		3.0f,
		false
	);
}

void UOCStoryWidget::SetStoryImage(UTexture2D* NewTexture)
{
	if (StoryImage && NewTexture)
	{
		StoryImage->SetBrushFromTexture(NewTexture, true);
	}
}

void UOCStoryWidget::EnableContinue()
{
	bCanContinue = true;

	if (ContinueButton)
	{
		ContinueButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (ContinueBlink)
	{
		PlayAnimation(
			ContinueBlink,
			0.0f,
			0
		);
	}
}

FReply UOCStoryWidget::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	if (!bCanContinue)
	{
		return FReply::Handled();
	}

	if (ContinueBlink)
	{
		StopAnimation(ContinueBlink);
	}

	SetVisibility(ESlateVisibility::Collapsed);

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}

	return FReply::Handled();
}