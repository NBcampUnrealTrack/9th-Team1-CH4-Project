#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"
#include "OCMenuWidget.generated.h"

class UButton;

UCLASS()
class OVERCOOKED_API UOCMenuWidget : public UOCBaseWidget
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