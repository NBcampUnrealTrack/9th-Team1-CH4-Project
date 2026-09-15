#include "InteractionComponent.h"
#include "../Character/APlayerCharacter.h"
#include "InteractableInterface.h"
#include "PickupableInterface.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"
#include "../Station/OverServingTableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Item/OverKitchenSettings.h"
#include "../Item/ItemHolderComponent.h"
#include "../Item/OverPickupItem.h"

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

    ActiveInteractTarget.Reset();

    AActor* Owner = GetOwner();
    UWorld* World = GetWorld();

    if (!Owner || !World)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("InteractionComponent Owner 또는 World가 없음")
        );

        return;
    }

    AAPlayerCharacter* Player =
        Cast<AAPlayerCharacter>(Owner);

    if (!Player)
    {
        return;
    }

    const FVector Start = Owner->GetActorLocation();
    const FVector End =
        Start + Owner->GetActorForwardVector() * InteractionDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    const UOverKitchenSettings* KitchenSettings =
        GetDefault<UOverKitchenSettings>();

    // 여러 개의 일반 테이블이 겹쳐 있어도 무한 반복하지 않도록 제한합니다.
    constexpr int32 MaxTableSkipCount = 8;

    for (int32 Attempt = 0; Attempt < MaxTableSkipCount; ++Attempt)
    {
        FHitResult Hit;

        const bool bHit = World->SweepSingleByChannel(
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
                TEXT("상호작용 대상 없음")
            );

            return;
        }

        AActor* Target = Hit.GetActor();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("상호작용 대상 발견: %s"),
            *Target->GetName()
        );

        // 서빙대는 일반 테이블 메시를 사용하더라도 먼저 처리합니다.
        if (UOverServingTableComponent* ServingComponent =
            Target->FindComponentByClass<UOverServingTableComponent>())
        {
            ServingComponent->TryServeHeldPlate(Player);
            return;
        }

		if (Target->GetClass()->ImplementsInterface(
			UInteractableInterface::StaticClass()
		))
        {
            ActiveInteractTarget = Target;

            IInteractableInterface::Execute_Interact(
                Target,
                Player
            );

            return;
        }

        UStaticMeshComponent* TargetMesh =
            Cast<UStaticMeshComponent>(Hit.GetComponent());

        if (!TargetMesh)
        {
            TargetMesh =
                Target->FindComponentByClass<UStaticMeshComponent>();
        }

        const bool bIsRegisteredTable =
            TargetMesh
            && KitchenSettings
            && KitchenSettings->IsTableMesh(
                TargetMesh->GetStaticMesh()
            );

        // 일반 테이블이 아니라면 벽처럼 정상적인 차단물로 취급합니다.
        if (!bIsRegisteredTable)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("상호작용을 막은 객체: %s"),
                *Target->GetName()
            );

            return;
        }

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("일반 테이블을 건너뛰고 다시 검사: %s"),
            *Target->GetName()
        );

        QueryParams.AddIgnoredActor(Target);
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("상호작용 테이블 검사 횟수 초과")
    );
}

void UInteractionComponent::StopInteract()
{
    AActor* Target = ActiveInteractTarget.Get();
    ActiveInteractTarget.Reset();

    AAPlayerCharacter* Player =
        Cast<AAPlayerCharacter>(GetOwner());

    if (!Target || !Player)
    {
        return;
    }

    if (Target->GetClass()->ImplementsInterface(
        UInteractableInterface::StaticClass()
    ))
    {
        IInteractableInterface::Execute_StopInteract(
            Target,
            Player
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

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FVector SphereCenter = Owner->GetActorLocation()
        + Owner->GetActorForwardVector() * PickupSphereForwardOffset;

    if (const ACharacter* Character = Cast<ACharacter>(Owner))
    {
        if (const UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
        {
            const float CapsuleBottom =
                Capsule->GetComponentLocation().Z - Capsule->GetScaledCapsuleHalfHeight();
            SphereCenter.Z = CapsuleBottom + PickupSphereHeight;
        }
    }

    const float SphereRadius = FMath::Max(PickupSphereRadius, 1.0f);

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Owner);

    TArray<FOverlapResult> Overlaps;
    World->OverlapMultiByObjectType(
        Overlaps,
        SphereCenter,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(SphereRadius),
        QueryParams
    );

    AActor* Target = nullptr;
    double ClosestDistanceSquared = TNumericLimits<double>::Max();
    TSet<AActor*> CheckedActors;

    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* Candidate = Overlap.GetActor();
        if (!Candidate || CheckedActors.Contains(Candidate))
        {
            continue;
        }

        CheckedActors.Add(Candidate);
        if (!Candidate->GetClass()->ImplementsInterface(
            UPickupableInterface::StaticClass()
        ))
        {
            continue;
        }

        const double DistanceSquared = FVector::DistSquared(
            SphereCenter,
            Candidate->GetActorLocation()
        );

        if (DistanceSquared < ClosestDistanceSquared)
        {
            ClosestDistanceSquared = DistanceSquared;
            Target = Candidate;
        }
    }

    if (bDrawPickupDebug)
    {
        DrawDebugSphere(
            World,
            SphereCenter,
            SphereRadius,
            24,
            Target ? FColor::Green : FColor::Red,
            false,
            PickupDebugDuration,
            0,
            2.0f
        );

        if (Target)
        {
            DrawDebugLine(
                World,
                SphereCenter,
                Target->GetActorLocation(),
                FColor::Green,
                false,
                PickupDebugDuration,
                0,
                2.0f
            );
        }
    }

    if (!Target)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("픽업 대상 없음")
        );

        return;
    }

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

bool UInteractionComponent::TryPlaceHeldItemOnTable()
{
    AAPlayerCharacter* Player =
        Cast<AAPlayerCharacter>(GetOwner());

    UWorld* World = GetWorld();

    if (!Player || !World || !Player->HasAuthority())
    {
        return false;
    }

    UItemHolderComponent* Holder =
        Player->FindComponentByClass<UItemHolderComponent>();

    AOverPickupItem* HeldItem =
        Holder
        ? Cast<AOverPickupItem>(Holder->GetHeldObject())
        : nullptr;

    if (!Holder || !HeldItem)
    {
        return false;
    }

    const FVector Start =
        Player->GetActorLocation();

    const FVector End =
        Start
        + Player->GetActorForwardVector()
        * InteractionDistance;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Player);
    QueryParams.AddIgnoredActor(HeldItem);

    FHitResult Hit;

    const bool bHit = World->SweepSingleByChannel(
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
        // 일반 탁자가 없으므로 기존 바닥 내려놓기를 실행합니다.
        return false;
    }

    AActor* Target = Hit.GetActor();

    // 접시 반환 전용 테이블과 접시꽂이에는 다른 아이템을 놓지 않습니다.
    if (Target->ActorHasTag(TEXT("PlateReturn")))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("접시 반환 전용 자리에는 다른 아이템을 놓을 수 없습니다.")
        );

        // 바닥으로 떨어뜨리지 않고 현재 아이템을 계속 들고 있습니다.
        return true;
    }

    // 앞에 다른 아이템이 있다면 서로 겹치지 않게 현재 아이템을 계속 듭니다.
    if (Target->IsA<AOverPickupItem>())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("탁자 배치 취소: 앞자리에 다른 아이템이 있음: %s"),
            *Target->GetName()
        );

        return true;
    }

    // 서빙대는 E키 일반 배치 대상이 아니라 F키 제출 대상입니다.
    if (Target->FindComponentByClass<
        UOverServingTableComponent>())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("서빙대는 F키로 사용합니다.")
        );

        return true;
    }

    UStaticMeshComponent* TableMesh =
        Cast<UStaticMeshComponent>(Hit.GetComponent());

    if (!TableMesh)
    {
        TableMesh =
            Target->FindComponentByClass<
            UStaticMeshComponent>();
    }

    const UOverKitchenSettings* KitchenSettings =
        GetDefault<UOverKitchenSettings>();

    const bool bIsRegisteredTable =
        TableMesh
        && KitchenSettings
        && KitchenSettings->IsTableMesh(
            TableMesh->GetStaticMesh()
        );

    if (!bIsRegisteredTable)
    {
        // 일반 탁자가 아니라면 기존 바닥 내려놓기를 실행합니다.
        return false;
    }

    // 이미 이 탁자에 부착된 아이템이 있는지 확인합니다.
    TArray<AActor*> AttachedActors;
    Target->GetAttachedActors(AttachedActors);

    for (AActor* AttachedActor : AttachedActors)
    {
        if (IsValid(
            Cast<AOverPickupItem>(AttachedActor)
        ))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("탁자 배치 취소: 탁자가 이미 사용 중: %s"),
                *Target->GetName()
            );

            return true;
        }
    }

    // 맵에서 직접 배치한 접시처럼 아직 탁자에 부착되지 않은 아이템도 확인합니다.
    const FBox TableBounds =
        TableMesh->Bounds.GetBox();

    const FVector SlotCheckCenter(
        TableBounds.GetCenter().X,
        TableBounds.GetCenter().Y,
        TableBounds.Max.Z + 30.0f
    );

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(
        ECC_WorldDynamic
    );
    ObjectQueryParams.AddObjectTypesToQuery(
        ECC_PhysicsBody
    );

    FCollisionQueryParams OccupancyQueryParams;
    OccupancyQueryParams.AddIgnoredActor(Player);
    OccupancyQueryParams.AddIgnoredActor(HeldItem);
    OccupancyQueryParams.AddIgnoredActor(Target);

    TArray<FOverlapResult> SlotOverlaps;

    World->OverlapMultiByObjectType(
        SlotOverlaps,
        SlotCheckCenter,
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(55.0f),
        OccupancyQueryParams
    );

    for (const FOverlapResult& Overlap : SlotOverlaps)
    {
        AOverPickupItem* ExistingItem =
            Cast<AOverPickupItem>(Overlap.GetActor());

        if (IsValid(ExistingItem))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("탁자 배치 취소: 탁자 위에 이미 %s 있음"),
                *ExistingItem->GetName()
            );

            return true;
        }
    }

    if (!HeldItem->PlaceOnTable(TableMesh))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("탁자 배치 실패: %s"),
            *HeldItem->GetName()
        );

        // 탁자는 찾았으므로 바닥으로 떨어뜨리지는 않습니다.
        return true;
    }

    if (!Holder->CompleteTransfer(HeldItem))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("탁자 배치 후 Holder 전달 정리 실패: %s"),
            *HeldItem->GetName()
        );

        return true;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("일반 탁자 배치 성공: %s -> %s"),
        *HeldItem->GetName(),
        *Target->GetName()
    );

    return true;
}
