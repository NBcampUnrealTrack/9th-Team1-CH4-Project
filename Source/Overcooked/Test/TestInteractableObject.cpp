// Fill out your copyright notice in the Description page of Project Settings.


#include "TestInteractableObject.h"


ATestInteractableObject::ATestInteractableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("Mesh")
	);

	RootComponent = Mesh;

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	Mesh->SetCollisionResponseToChannel(
		ECC_Visibility,
		ECR_Block
	);
}


void ATestInteractableObject::Interact_Implementation(AAPlayerCharacter* Player)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("TestInteractableObject 상호작용 성공")
	);
}