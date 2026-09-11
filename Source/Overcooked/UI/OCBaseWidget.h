#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OCBaseWidget.generated.h"

UCLASS()
class OVERCOOKED_API UOCBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ShowWidget();

	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void HideWidget();
};