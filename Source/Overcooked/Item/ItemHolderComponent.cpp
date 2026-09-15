// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemHolderComponent.h"
#include "KitchenObject.h"
#include "../Character/APlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Item/OverPickupItem.h"


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
			TEXT("ItemHolderComponent: 들 객체가 없음")
		);

		return;
	}

	if (HeldObject)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: 이미 객체를 들고 있음")
		);

		return;
	}

	AAPlayerCharacter* Player =
		Cast<AAPlayerCharacter>(GetOwner());

	if (!Player)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: Owner가 Player가 아님")
		);

		return;
	}

	USkeletalMeshComponent* PlayerMesh =
		Player->GetMesh();

	if (!PlayerMesh)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ItemHolderComponent: Player Mesh가 없음")
		);

		return;
	}

	if (AOverPickupItem* PickupItem =
		Cast<AOverPickupItem>(Object))
	{
		if (!PickupItem->PickUp(
			PlayerMesh,
			Player,
			TEXT("HoldSocket")
		))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("ItemHolderComponent: 픽업 처리 실패")
			);

			return;
		}
	}
	else
	{
		// 기존 AIngredient 같은 일반 KitchenObject 처리
		Object->DisableComponentsSimulatePhysics();
		Object->SetActorEnableCollision(false);

		Object->AttachToComponent(
			PlayerMesh,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("HoldSocket")
		);
	}

	// 실제 부착이 성공한 뒤에 보관합니다.
	HeldObject = Object;
	Player->SetIsCarryingItem(true);

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

	AAPlayerCharacter* Player =
		Cast<AAPlayerCharacter>(GetOwner());

	if (!Player)
	{
		return;
	}

	const UCapsuleComponent* Capsule =
		Player->GetCapsuleComponent();

	if (!Capsule)
	{
		return;
	}

	AKitchenObject* ReleasedObject = HeldObject;

	FVector BoundsOrigin;
	FVector BoundsExtent;
	ReleasedObject->GetActorBounds(
		false,
		BoundsOrigin,
		BoundsExtent
	);

	const float ActorToBottom =
		ReleasedObject->GetActorLocation().Z
		- (BoundsOrigin.Z - BoundsExtent.Z);

	FVector DropLocation =
		Player->GetActorLocation()
		+ Player->GetActorForwardVector() * 120.0f;

	DropLocation.Z =
		Capsule->GetComponentLocation().Z
		- Capsule->GetScaledCapsuleHalfHeight()
		+ ActorToBottom;

	if (AOverPickupItem* PickupItem =
		Cast<AOverPickupItem>(ReleasedObject))
	{
		// AOverPickupItem의 상태·물리·충돌 처리를 사용합니다.
		PickupItem->Drop(
			DropLocation,
			Player->GetActorRotation()
		);
	}
	else
	{
		// 기존 AIngredient 같은 일반 KitchenObject 처리
		ReleasedObject->DetachFromActor(
			FDetachmentTransformRules::KeepWorldTransform
		);

		ReleasedObject->SetActorLocation(
			DropLocation,
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);

		ReleasedObject->SetActorEnableCollision(true);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("ItemHolderComponent: %s 놓기"),
		*ReleasedObject->GetName()
	);

	HeldObject = nullptr;
	Player->SetIsCarryingItem(false);
}


bool UItemHolderComponent::CompleteTransfer(AKitchenObject* Object)
{
	if (!Object || HeldObject != Object || !GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}

	HeldObject = nullptr;

	if (AAPlayerCharacter* Player = Cast<AAPlayerCharacter>(GetOwner()))
	{
		Player->SetIsCarryingItem(false);
	}

	UE_LOG(LogTemp, Log, TEXT("ItemHolderComponent: %s 배치 지점으로 전달"), *Object->GetName());

	return true;
}




AKitchenObject* UItemHolderComponent::GetHeldObject() const
{
	return HeldObject;
}

