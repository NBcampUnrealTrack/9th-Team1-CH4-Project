#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"
#include "OCResultWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UOverlay;
class UTexture2D;

UCLASS()
class OVERCOOKED_API UOCResultWidget : public UOCBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetStageText(const FString& StageName);

	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetResultValues(
		int32 DeliveredValue,
		int32 TipValue,
		int32 FailedValue,
		int32 TotalScore
	);

	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetStarThresholds(
		int32 CurrentScore,
		int32 OneStarScore,
		int32 TwoStarScore,
		int32 ThreeStarScore
	);

	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetPlayerCount(int32 PlayerCount);

	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetPlayer1Name(const FString& PlayerName);

	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetPlayer2Name(const FString& PlayerName);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void OnNextClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Result")
	void BP_OnRetryClicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Result")
	void BP_OnNextClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DeliveredValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TipValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> FailedValueText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Star1ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Star2ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Star3ScoreText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star1Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star2Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star3Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Player1Panel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Player2Panel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player1NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player2NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RetryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;
};