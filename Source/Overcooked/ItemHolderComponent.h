// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemHolderComponent.generated.h"


class AKitchenObject;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERCOOKED_API UItemHolderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemHolderComponent();

	void Hold(AKitchenObject* Object);
	void Release();
	bool CompleteTransfer(AKitchenObject* Object);


	AKitchenObject* GetHeldObject() const;

private:
	UPROPERTY()
	TObjectPtr<AKitchenObject> HeldObject;
};
