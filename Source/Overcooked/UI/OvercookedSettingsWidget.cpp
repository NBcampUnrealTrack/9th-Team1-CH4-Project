#include "OvercookedSettingsWidget.h"

#include "Components/Slider.h"
#include "Components/Button.h"

void UOvercookedSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(
			this,
			&UOvercookedSettingsWidget::OnMasterVolumeChanged
		);
	}

	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->OnValueChanged.AddDynamic(
			this,
			&UOvercookedSettingsWidget::OnBGMVolumeChanged
		);
	}

	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(
			this,
			&UOvercookedSettingsWidget::OnSFXVolumeChanged
		);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(
			this,
			&UOvercookedSettingsWidget::OnBackButtonClicked
		);
	}
}

void UOvercookedSettingsWidget::OnMasterVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("Master Volume: %f"), Value);
}

void UOvercookedSettingsWidget::OnBGMVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("BGM Volume: %f"), Value);
}

void UOvercookedSettingsWidget::OnSFXVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("SFX Volume: %f"), Value);
}

void UOvercookedSettingsWidget::OnBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Back Button Clicked"));

	HideWidget();
}