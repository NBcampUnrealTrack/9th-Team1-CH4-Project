#pragma once

#include "CoreMinimal.h"
#include "OCBaseWidget.h"
#include "OCMenuWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UTexture2D;

UCLASS()
class OVERCOOKED_API UOCMenuWidget : public UOCBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Overcooked|Menu")
	void SetPlayer2Connected(bool bConnected);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> HostButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> JoinButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Player2Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Player2NameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Menu")
	TObjectPtr<UTexture2D> Player2WaitingTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Overcooked|Menu")
	TObjectPtr<UTexture2D> Player2PandaTexture;

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnSettingsButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

	UFUNCTION()
	void OnConnectionButtonClicked();
		
	};
