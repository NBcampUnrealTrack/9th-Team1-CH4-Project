#pragma once

#include "CoreMinimal.h"
#include "OvercookedBaseWidget.h"
#include "OvercookedHUDWidget.generated.h"

class UTextBlock;

UCLASS()
class OVERCOOKED_API UOvercookedHUDWidget : public UOvercookedBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetRemainingTime(float NewTime);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TimerText;
};