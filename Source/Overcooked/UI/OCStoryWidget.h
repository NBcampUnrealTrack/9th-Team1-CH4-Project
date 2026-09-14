#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"
#include "OCStoryWidget.generated.h"

class UImage;
class UButton;
class UTexture2D;
class UWidgetAnimation;

UCLASS()
class OVERCOOKED_API UOCStoryWidget : public UOCBaseWidget
{
	GENERATED_BODY()

public:
	UOCStoryWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Story")
	void SetStoryImage(UTexture2D* NewTexture);

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> StoryImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ContinueBlink;

private:
	void EnableContinue();

	FTimerHandle ContinueTimerHandle;

	bool bCanContinue = false;
};