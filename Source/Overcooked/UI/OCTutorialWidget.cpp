#include "OCTutorialWidget.h"

#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

UOCTutorialWidget::UOCTutorialWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UOCTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());

		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}

	SetKeyboardFocus();
}

void UOCTutorialWidget::SetTutorialImage(UTexture2D* NewTexture)
{
	if (TutorialImage && NewTexture)
	{
		TutorialImage->SetBrushFromTexture(NewTexture, true);
	}
}

FReply UOCTutorialWidget::NativeOnKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Tutorial Key Pressed: %s"),
		*InKeyEvent.GetKey().ToString()
	);

	if (TutorialHide)
	{
		PlayAnimation(TutorialHide);

		FTimerHandle HideTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			HideTimerHandle,
			[this]()
			{
				SetVisibility(ESlateVisibility::Collapsed);

				if (APlayerController* PC = GetOwningPlayer())
				{
					FInputModeGameOnly InputMode;
					PC->SetInputMode(InputMode);
				}
			},
			0.35f,
			false
		);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TutorialHide animation is NULL!")
		);
	}

	return FReply::Handled();
}