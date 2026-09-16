#include "OCSettingsWidget.h"

#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

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
	UE_LOG(LogTemp, Warning, TEXT("Slider Value = %f"), Value);

	if (MasterVolumeProgressBar)
	{
		MasterVolumeProgressBar->SetPercent(Value);

		UE_LOG(LogTemp, Warning, TEXT("ProgressBar Percent = %f"), Value);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MasterVolumeProgressBar IS NULL"));
	}

	if (!SettingsSoundMix || !MasterSoundClass)
	{
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(
		this,
		SettingsSoundMix,
		MasterSoundClass,
		Value,
		1.0f,
		0.0f,
		true
	);

	UGameplayStatics::PushSoundMixModifier(
		this,
		SettingsSoundMix
	);
}
void UOCSettingsWidget::OnBGMVolumeChanged(float Value)
{
	if (!SettingsSoundMix || !BGMSoundClass)
	{
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(
		this,
		SettingsSoundMix,
		BGMSoundClass,
		Value,
		1.0f,
		0.0f,
		true
	);

	UGameplayStatics::PushSoundMixModifier(
		this,
		SettingsSoundMix
	);
}
void UOCSettingsWidget::OnSFXVolumeChanged(float Value)
{
	if (!SettingsSoundMix || !SFXSoundClass)
	{
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(
		this,
		SettingsSoundMix,
		SFXSoundClass,
		Value,
		1.0f,
		0.0f,
		true
	);

	UGameplayStatics::PushSoundMixModifier(
		this,
		SettingsSoundMix
	);
}

void UOCSettingsWidget::OnBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Back Button Clicked"));

	HideWidget();
}