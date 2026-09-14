// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class AAPlayerCharacter;

UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/*
  상호작용 가능한 객체가 구현해야 하는 인터페이스
 */
class OVERCOOKED_API IInteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void Interact(AAPlayerCharacter* Player);

	UFUNCTION(BlueprintNativeEvent)
	void StopInteract(AAPlayerCharacter* Player);
	
};