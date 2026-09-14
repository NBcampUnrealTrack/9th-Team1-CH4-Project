// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Item/KitchenObject.h"
#include "TestInteractableObject.generated.h"

UCLASS()
class OVERCOOKED_API ATestInteractableObject : public AKitchenObject
{
	GENERATED_BODY()

public:
	
	ATestInteractableObject();
	
	virtual void Interact_Implementation(AAPlayerCharacter* Player) override;
	
	private:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
};
