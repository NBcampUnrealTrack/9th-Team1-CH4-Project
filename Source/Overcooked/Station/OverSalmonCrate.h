#pragma once

#include "CoreMinimal.h"
#include "OverIngredientCrate.h"
#include "OverSalmonCrate.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class OVERCOOKED_API AOverSalmonCrate : public AOverIngredientCrate
{
	GENERATED_BODY()

public:
	AOverSalmonCrate();

private:
	UPROPERTY(VisibleAnywhere, Category = "Crate")
	TObjectPtr<UBoxComponent> BlockingBox;
};
