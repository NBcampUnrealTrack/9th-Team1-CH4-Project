#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"

#include "OCHUDWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UTexture2D;
class UProgressBar;
class UWidgetAnimation;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimeOver);

UCLASS()
class OVERCOOKED_API UOCHUDWidget : public UOCBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetScore(int32 NewScore);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetRemainingTime(float NewTime);

	UFUNCTION(BlueprintCallable, Category = "HUD|Order")
	void AddOrder(UTexture2D* OrderTexture);

	UFUNCTION(BlueprintCallable, Category = "HUD|Order")
	void ClearOrders();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTimer(float RemainingTime, float TotalTime);
	
	UPROPERTY(BlueprintAssignable, Category = "HUD")
	FOnTimeOver OnTimeOver;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> OrderBox;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> TimeProgressBar;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> TimerWarningShake;

	bool bIsTimerWarning = false;
	bool bTimeOverTriggered = false;
	
};