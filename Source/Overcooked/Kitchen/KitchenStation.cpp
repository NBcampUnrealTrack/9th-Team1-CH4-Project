// Fill out your copyright notice in the Description page of Project Settings.


#include "KitchenStation.h"


// Sets default values
AKitchenStation::AKitchenStation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AKitchenStation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKitchenStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

