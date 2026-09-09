// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "KitchenObject.generated.h"

class AAPlayerCharacter;

UCLASS()
class OVERCOOKED_API AKitchenObject : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AKitchenObject();
	void Interact(AAPlayerCharacter* Player);

	virtual void Interact_Implementation(AAPlayerCharacter* Player) override;
};