#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OvercookedBaseWidget.generated.h"

UCLASS()
class OVERCOOKED_API UOvercookedBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ShowWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void HideWidget();
};