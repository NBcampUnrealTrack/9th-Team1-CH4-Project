#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OCConnectionWidget.generated.h"

class UButton;
class UEditableTextBox;
class UOCMenuWidget;
class UVerticalBox;

UCLASS()
class OVERCOOKED_API UOCConnectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeConnectionScreen(UOCMenuWidget* InSourceMenu);

protected:
	virtual void NativeOnInitialized() override;

private:
	UButton* CreateTextButton(const FText& Label);

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnConnectClicked();

	UFUNCTION()
	void OnBackClicked();

	UPROPERTY()
	TObjectPtr<UVerticalBox> AddressPanel;

	UPROPERTY()
	TObjectPtr<UEditableTextBox> AddressInput;

	UPROPERTY()
	TObjectPtr<UOCMenuWidget> SourceMenu;
};
