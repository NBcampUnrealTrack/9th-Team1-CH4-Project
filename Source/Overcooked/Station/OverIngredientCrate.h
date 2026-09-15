#pragma once

#include "CoreMinimal.h"
#include "../Item/KitchenObject.h"
#include "OverIngredientCrate.generated.h"

class AOverPickupItem;
class UStaticMeshComponent;

UCLASS(Abstract, Blueprintable)
class OVERCOOKED_API AOverIngredientCrate : public AKitchenObject
{
	GENERATED_BODY()

public:
	AOverIngredientCrate();
	virtual void Interact_Implementation(AAPlayerCharacter* Player) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crate")
	TObjectPtr<UStaticMeshComponent> CrateMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crate")
	TSubclassOf<AOverPickupItem> IngredientClass;
};
