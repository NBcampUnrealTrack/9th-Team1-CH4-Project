#pragma once

#include "CoreMinimal.h"
#include "OverIngredientCrate.h"
#include "OverOctopusCrate.generated.h"

class UBoxComponent;

UCLASS(Blueprintable)
class OVERCOOKED_API AOverOctopusCrate : public AOverIngredientCrate
{
	GENERATED_BODY()

public:
	AOverOctopusCrate();

private:
	UPROPERTY(VisibleAnywhere, Category = "Crate")
	TObjectPtr<UBoxComponent> BlockingBox;
};
