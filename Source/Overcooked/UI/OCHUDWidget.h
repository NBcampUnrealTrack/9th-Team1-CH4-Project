#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"

#include "OCHUDWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UTexture2D;
class UProgressBar;
class UWidgetAnimation;
class UImage;

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
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTipMultiplier(float Multiplier);
	
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetGameplayHUDVisible(bool bVisible);
	
	UFUNCTION(BlueprintCallable, Category = "Countdown")
	void SetCountdown(int32 Count);

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TipMultiplierText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CountdownNumberImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CountdownStartImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CountdownStartPop;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> CountdownNumberPop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Countdown")
	TObjectPtr<UTexture2D> CountdownTexture3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Countdown")
	TObjectPtr<UTexture2D> CountdownTexture2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Countdown")
	TObjectPtr<UTexture2D> CountdownTexture1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Countdown")
	TObjectPtr<UTexture2D> CountdownTextureStart;
	
	int32 LastCountdownValue = -1;
	
	bool bIsTimerWarning = false;
	bool bTimeOverTriggered = false;
	
};