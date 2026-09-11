#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"
#include "OCHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class OVERCOOKED_API UOCHUDWidget : public UOCBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetRemainingTime(float NewTime);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
};