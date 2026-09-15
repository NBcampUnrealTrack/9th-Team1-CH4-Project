#pragma once

#include "CoreMinimal.h"
#include "OverIngredientCrate.h"
#include "OverShrimpCrate.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class OVERCOOKED_API AOverShrimpCrate : public AOverIngredientCrate
{
	GENERATED_BODY()

public:
	AOverShrimpCrate();

private:
	UPROPERTY(VisibleAnywhere, Category = "Crate")
	TObjectPtr<UBoxComponent> BlockingBox;
};
