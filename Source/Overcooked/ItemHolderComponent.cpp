// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemHolderComponent.h"
#include "KitchenObject.h"
#include "APlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"


UItemHolderComponent::UItemHolderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemHolderComponent::Hold(AKitchenObject* Object)
{
	if (!Object)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: 들고 있는 객체가 없음")
		);

		return;
	}

	HeldObject = Object;

	AAPlayerCharacter* Player = Cast<AAPlayerCharacter>(GetOwner());

	if (!Player)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: Owner가 Player가 아님")
		);

		return;
	}
	
	USkeletalMeshComponent* PlayerMesh = Player->GetMesh();

	if (!PlayerMesh)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: Player Mesh가 없음")
		);

		return;
	}
	
	Object->AttachToComponent(
	PlayerMesh,
	FAttachmentTransformRules::SnapToTargetNotIncludingScale,TEXT("HoldSocket")
);
	
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("ItemHolderComponent: %s 들기"),
		*Object->GetName()
	);
}

void UItemHolderComponent::Release()
{
	if (!HeldObject)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: 놓을 객체가 없음")
		);

		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("ItemHolderComponent: %s 놓기"),
		*HeldObject->GetName()
	);

	HeldObject = nullptr;
}





AKitchenObject* UItemHolderComponent::GetHeldObject() const
{
	return HeldObject;
}

