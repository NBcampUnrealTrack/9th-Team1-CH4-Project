#include "OCSettingsWidget.h"

#include "Components/Slider.h"
#include "Components/Button.h"

void UOCSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(
			this,
			&UOCSettingsWidget::OnMasterVolumeChanged
		);
	}

	if (BGMVolumeSlider)
	{
		BGMVolumeSlider->OnValueChanged.AddDynamic(
			this,
			&UOCSettingsWidget::OnBGMVolumeChanged
		);
	}

	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(
			this,
			&UOCSettingsWidget::OnSFXVolumeChanged
		);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(
			this,
			&UOCSettingsWidget::OnBackButtonClicked
		);
	}
}

void UOCSettingsWidget::OnMasterVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("Master Volume: %f"), Value);
}

void UOCSettingsWidget::OnBGMVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("BGM Volume: %f"), Value);
}

void UOCSettingsWidget::OnSFXVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("SFX Volume: %f"), Value);
}

void UOCSettingsWidget::OnBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Back Button Clicked"));

	HideWidget();
}