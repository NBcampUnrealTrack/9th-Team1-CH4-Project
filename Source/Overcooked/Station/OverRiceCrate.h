#pragma once

#include "CoreMinimal.h"
#include "OverIngredientCrate.h"
#include "OverRiceCrate.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class OVERCOOKED_API AOverRiceCrate : public AOverIngredientCrate
{
	GENERATED_BODY()

public:
	AOverRiceCrate();

private:
	UPROPERTY(VisibleAnywhere, Category = "Crate")
	TObjectPtr<UBoxComponent> BlockingBox;
};
