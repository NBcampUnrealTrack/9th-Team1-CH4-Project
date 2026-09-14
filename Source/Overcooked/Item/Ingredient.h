// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KitchenObject.h"
#include "../Interaction/PickupableInterface.h"
#include "Ingredient.generated.h"


class AAPlayerCharacter;

UCLASS()
class OVERCOOKED_API AIngredient : public AKitchenObject, public IPickupableInterface
{
	GENERATED_BODY()

public:
	AIngredient();

	virtual void Interact_Implementation(
		AAPlayerCharacter* Player
	) override;
	void Pickup_Implementation(AAPlayerCharacter* Player);
};
