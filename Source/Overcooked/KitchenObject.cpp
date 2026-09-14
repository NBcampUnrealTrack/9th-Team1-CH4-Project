// Fill out your copyright notice in the Description page of Project Settings.


#include "KitchenObject.h"


AKitchenObject::AKitchenObject()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicatingMovement(true);
}

void AKitchenObject::Interact_Implementation(
	AAPlayerCharacter* Player
)
{
	UE_LOG(LogTemp,Warning,TEXT("KitchenObject Interact 호출")
	);
}
