#include "InteractionComponent.h"
#include "APlayerCharacter.h"
#include "InteractableInterface.h"
#include "PickupableInterface.h"
#include "GameFramework/Actor.h"

UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::TryInteract()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("InteractionComponent::TryInteract 호출")
    );

    AActor* Owner = GetOwner();

    if (!Owner)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("InteractionComponent Owner가 없음")
        );

        return;
    }

    const FVector Start = Owner->GetActorLocation();

    const FVector End =
        Start + Owner->GetActorForwardVector() * InteractionDistance;

    FHitResult Hit;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    const bool bHit = GetWorld()->SweepSingleByChannel(
        Hit,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(50.0f),
        QueryParams
    );

    if (bHit && Hit.GetActor())
    {
        AActor* Target = Hit.GetActor();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("상호작용 대상 발견: %s"),
            *Target->GetName()
        );

        if (Target->GetClass()->ImplementsInterface(
            UInteractableInterface::StaticClass()
        ))
        {
            IInteractableInterface::Execute_Interact(
                Target,
                Cast<AAPlayerCharacter>(Owner)
            );
        }
    }
    else
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("상호작용 대상 없음")
        );
    }
}

void UInteractionComponent::TryPickup()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("InteractionComponent::TryPickup 호출")
    );

    AActor* Owner = GetOwner();

    if (!Owner)
    {
        return;
    }

    const FVector Start = Owner->GetActorLocation();

    const FVector End =
        Start + Owner->GetActorForwardVector() * InteractionDistance;

    FHitResult Hit;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    const bool bHit = GetWorld()->SweepSingleByChannel(
        Hit,
        Start,
        End,
        FQuat::Identity,
        ECC_Visibility,
        FCollisionShape::MakeSphere(50.0f),
        QueryParams
    );

    if (!bHit || !Hit.GetActor())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("픽업 대상 없음")
        );

        return;
    }

    AActor* Target = Hit.GetActor();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("픽업 대상 발견: %s"),
        *Target->GetName()
    );

    if (Target->GetClass()->ImplementsInterface(
        UPickupableInterface::StaticClass()
    ))
    {
        IPickupableInterface::Execute_Pickup(
            Target,
            Cast<AAPlayerCharacter>(Owner)
        );
    }
    else
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("픽업 불가능 객체")
        );
    }
}