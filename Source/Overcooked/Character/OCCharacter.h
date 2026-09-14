#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OCCharacter.generated.h"

class UWidgetComponent;

UCLASS(Blueprintable)
class OVERCOOKED_API AOCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOCCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Overcooked|UI")
	TObjectPtr<UWidgetComponent> InvalidOrderWidgetComponent;
};