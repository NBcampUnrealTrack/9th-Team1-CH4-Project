#pragma once

#include "CoreMinimal.h"
#include "OvercookedBaseWidget.h"
#include "OvercookedMenuWidget.generated.h"

class UButton;

UCLASS()
class OVERCOOKED_API UOvercookedMenuWidget : public UOvercookedBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnSettingsButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();
};