// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KitchenStation.h"
#include "Counter.generated.h"

UCLASS()
class OVERCOOKED_API ACounter : public AKitchenStation
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACounter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
