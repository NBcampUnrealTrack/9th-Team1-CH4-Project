#pragma once

#include "CoreMinimal.h"
#include "OvercookedBaseWidget.h"
#include "OvercookedSettingsWidget.generated.h"

class USlider;
class UButton;

UCLASS()
class OVERCOOKED_API UOvercookedSettingsWidget : public UOvercookedBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> BGMVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SFXVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnBGMVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void OnBackButtonClicked();
};