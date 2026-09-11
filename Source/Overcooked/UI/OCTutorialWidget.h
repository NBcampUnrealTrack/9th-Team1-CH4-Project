#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"
#include "OCTutorialWidget.generated.h"

class UImage;
class UTexture2D;
class UWidgetAnimation;

UCLASS()
class OVERCOOKED_API UOCTutorialWidget : public UOCBaseWidget
{
	GENERATED_BODY()

public:
	UOCTutorialWidget(const FObjectInitializer& ObjectInitializer);

	// 스테이지마다 튜토리얼 이미지 교체
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SetTutorialImage(UTexture2D* NewTexture);

protected:
	virtual void NativeConstruct() override;

	// 아무 키 입력 처리
	virtual FReply NativeOnKeyDown(
		const FGeometry& InGeometry,
		const FKeyEvent& InKeyEvent
	) override;

	// WBP_Tutorial의 TutorialImage
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> TutorialImage;

	// WBP_Tutorial의 TutorialHide 애니메이션
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> TutorialHide;
};