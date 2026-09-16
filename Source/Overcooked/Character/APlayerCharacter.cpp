// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequence.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Interaction/InteractionComponent.h"
#include "../Item/ItemHolderComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

#include "InputAction.h"
#include "InputCoreTypes.h"
#include "UObject/ConstructorHelpers.h"




AAPlayerCharacter::AAPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm =
        CreateDefaultSubobject<USpringArmComponent>(
            TEXT("SpringArm")
        );

    SpringArm->SetupAttachment(RootComponent);

    SpringArm->TargetArmLength = 1000.0f;

    SpringArm->SetRelativeRotation(
        FRotator(-60.0f, 0.0f, 0.0f)
    );

    SpringArm->bUsePawnControlRotation = false;
    
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;


    Camera =
        CreateDefaultSubobject<UCameraComponent>(
            TEXT("Camera")
        );

    Camera->SetupAttachment(
        SpringArm,
        USpringArmComponent::SocketName
    );

    Camera->bUsePawnControlRotation = false;

    InteractionComponent =
    CreateDefaultSubobject<UInteractionComponent>(
        TEXT("InteractionComponent")
    );
    
    ItemHolderComponent =
    CreateDefaultSubobject<UItemHolderComponent>(
        TEXT("ItemHolderComponent")
    );

    ChoppingKnifeMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("ChoppingKnifeMesh")
        );

    // KnifeSocket은 Panda_Skeleton의 오른손에 별도로 생성합니다.
    ChoppingKnifeMesh->SetupAttachment(
        GetMesh(),
        TEXT("KnifeSocket")
    );

    ChoppingKnifeMesh->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );

    ChoppingKnifeMesh->SetSimulatePhysics(false);
    ChoppingKnifeMesh->SetVisibility(false, true);
    ChoppingKnifeMesh->SetHiddenInGame(true, true);

    static ConstructorHelpers::FObjectFinder<UStaticMesh>
        ChoppingKnifeAsset(
            TEXT("/Game/External/Kenney/FoodKit/Food/cooking-knife-chopping.cooking-knife-chopping")
        );

    if (ChoppingKnifeAsset.Succeeded())
    {
        ChoppingKnifeMesh->SetStaticMesh(
            ChoppingKnifeAsset.Object
        );
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        ChefCarryAnimationAsset(
            TEXT("/Game/External/Quaternius/UltimateAnimatedCharacters/Chefs/Animations/Chef_MaleCharacterArmature_Walk_Carry.Chef_MaleCharacterArmature_Walk_Carry")
        );

    if (ChefCarryAnimationAsset.Succeeded())
    {
        ChefCarryAnimation = ChefCarryAnimationAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        PandaCarryAnimationAsset(
            TEXT("/Game/External/Quaternius/SushiRestaurant/Characters/Animations/PandaCharacterArmature_Walk_Holding.PandaCharacterArmature_Walk_Holding")
        );

    if (PandaCarryAnimationAsset.Succeeded())
    {
        PandaCarryAnimation = PandaCarryAnimationAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        PandaIdleCarryAnimationAsset(
            TEXT("/Game/External/Quaternius/SushiRestaurant/Characters/Animations/PandaCharacterArmature_Idle_Holding.PandaCharacterArmature_Idle_Holding")
        );

    if (PandaIdleCarryAnimationAsset.Succeeded())
    {
        PandaIdleCarryAnimation = PandaIdleCarryAnimationAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        ChefRunAnimationAsset(
            TEXT("/Game/External/Quaternius/UltimateAnimatedCharacters/Chefs/Animations/Chef_MaleCharacterArmature_Run.Chef_MaleCharacterArmature_Run")
        );
    ChefRunAnimation = ChefRunAnimationAsset.Object;

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        ChefRunCarryAnimationAsset(
            TEXT("/Game/External/Quaternius/UltimateAnimatedCharacters/Chefs/Animations/Chef_MaleCharacterArmature_Run_Carry.Chef_MaleCharacterArmature_Run_Carry")
        );
    ChefRunCarryAnimation = ChefRunCarryAnimationAsset.Object;

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        PandaRunAnimationAsset(
            TEXT("/Game/External/Quaternius/SushiRestaurant/Characters/Animations/PandaCharacterArmature_Run.PandaCharacterArmature_Run")
        );
    PandaRunAnimation = PandaRunAnimationAsset.Object;

    static ConstructorHelpers::FObjectFinder<UAnimSequence>
        PandaRunCarryAnimationAsset(
            TEXT("/Game/External/Quaternius/SushiRestaurant/Characters/Animations/PandaCharacterArmature_Run_Holding.PandaCharacterArmature_Run_Holding")
        );
    PandaRunCarryAnimation = PandaRunCarryAnimationAsset.Object;

    InvalidOrderWidgetComponent =
    CreateDefaultSubobject<UWidgetComponent>(
        TEXT("InvalidOrderWidget")
    );

    InvalidOrderWidgetComponent->SetupAttachment(RootComponent);

    InvalidOrderWidgetComponent->SetRelativeLocation(
        FVector(0.0f, 0.0f, 140.0f)
    );

    InvalidOrderWidgetComponent->SetWidgetSpace(
        EWidgetSpace::Screen
    );

    InvalidOrderWidgetComponent->SetDrawSize(
        FVector2D(160.0f, 160.0f)
    );

    InvalidOrderWidgetComponent->SetVisibility(false);
    
    
    bUseControllerRotationYaw = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;

    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 720.0f, 0.0f);
    
}


void AAPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    DefaultAnimInstanceClass = GetMesh()
        ? GetMesh()->GetAnimClass()
        : nullptr;
    WalkingSpeed = GetCharacterMovement()
        ? GetCharacterMovement()->MaxWalkSpeed
        : 0.0f;

    if (APlayerController* PlayerController =
        Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<
                UEnhancedInputLocalPlayerSubsystem
            >(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(
                PlayerMappingContext,
                0
            );
        }
    }
}

bool AAPlayerCharacter::IsChopping() const
{
    return bIsChopping;
}

void AAPlayerCharacter::SetIsChopping(bool bNewIsChopping)
{
    if (!HasAuthority() || bIsChopping == bNewIsChopping)
    {
        return;
    }

    bIsChopping = bNewIsChopping;
    ForceNetUpdate();
}

void AAPlayerCharacter::ShowChoppingKnife()
{
    if (!ChoppingKnifeMesh)
    {
        return;
    }

    ChoppingKnifeMesh->SetHiddenInGame(false, true);
    ChoppingKnifeMesh->SetVisibility(true, true);
}

void AAPlayerCharacter::HideChoppingKnife()
{
    if (!ChoppingKnifeMesh)
    {
        return;
    }

    ChoppingKnifeMesh->SetVisibility(false, true);
    ChoppingKnifeMesh->SetHiddenInGame(true, true);
}

void AAPlayerCharacter::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AAPlayerCharacter, bIsChopping);
    DOREPLIFETIME(AAPlayerCharacter, bIsCarryingItem);
    DOREPLIFETIME(AAPlayerCharacter, bIsRunning);
}

void AAPlayerCharacter::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    ApplyCarryingAnimation();
}

void AAPlayerCharacter::SetIsCarryingItem(const bool bNewIsCarryingItem)
{
    if (!HasAuthority() || bIsCarryingItem == bNewIsCarryingItem)
    {
        return;
    }

    bIsCarryingItem = bNewIsCarryingItem;
    ApplyCarryingAnimation();
    ForceNetUpdate();
}

void AAPlayerCharacter::OnRep_IsCarryingItem()
{
    ApplyCarryingAnimation();
}

void AAPlayerCharacter::ApplyCarryingAnimation()
{
    USkeletalMeshComponent* CharacterMesh = GetMesh();
    if (!CharacterMesh)
    {
        return;
    }

    const USkeleton* CharacterSkeleton = CharacterMesh->GetSkeletalMeshAsset()
        ? CharacterMesh->GetSkeletalMeshAsset()->GetSkeleton()
        : nullptr;
    const bool bIsMoving = GetVelocity().SizeSquared2D() > FMath::Square(5.0f);
    UAnimSequence* DesiredAnimation = nullptr;

    if (bIsMoving
        && bIsRunning
        && bIsCarryingItem
        && ChefRunCarryAnimation
        && ChefRunCarryAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = ChefRunCarryAnimation;
    }
    else if (bIsMoving
        && bIsRunning
        && bIsCarryingItem
        && PandaRunCarryAnimation
        && PandaRunCarryAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = PandaRunCarryAnimation;
    }
    else if (bIsMoving
        && bIsCarryingItem
        && ChefCarryAnimation
        && ChefCarryAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = ChefCarryAnimation;
    }
    else if (bIsMoving
        && bIsCarryingItem
        && PandaCarryAnimation
        && PandaCarryAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = PandaCarryAnimation;
    }
    else if (!bIsMoving
        && bIsCarryingItem
        && PandaIdleCarryAnimation
        && PandaIdleCarryAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = PandaIdleCarryAnimation;
    }
    else if (!bIsMoving
        && bIsCarryingItem
        && ChefCarryAnimation
        && ChefCarryAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = ChefCarryAnimation;
    }
    else if (bIsMoving
        && bIsRunning
        && ChefRunAnimation
        && ChefRunAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = ChefRunAnimation;
    }
    else if (bIsMoving
        && bIsRunning
        && PandaRunAnimation
        && PandaRunAnimation->GetSkeleton() == CharacterSkeleton)
    {
        DesiredAnimation = PandaRunAnimation;
    }

    if (DesiredAnimation)
    {
        if (ActiveOverrideAnimation != DesiredAnimation)
        {
            CharacterMesh->PlayAnimation(DesiredAnimation, true);
            ActiveOverrideAnimation = DesiredAnimation;
        }
        return;
    }

    if (ActiveOverrideAnimation)
    {
        ActiveOverrideAnimation = nullptr;
        CharacterMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
        CharacterMesh->SetAnimInstanceClass(DefaultAnimInstanceClass);
    }
}


void AAPlayerCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent
)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Pressed, this, &AAPlayerCharacter::StartRunning);
    PlayerInputComponent->BindKey(EKeys::LeftShift, IE_Released, this, &AAPlayerCharacter::StopRunning);
    PlayerInputComponent->BindKey(EKeys::RightShift, IE_Pressed, this, &AAPlayerCharacter::StartRunning);
    PlayerInputComponent->BindKey(EKeys::RightShift, IE_Released, this, &AAPlayerCharacter::StopRunning);

    if (UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Started,
            this,
            &AAPlayerCharacter::Interact
        );

        EnhancedInputComponent->BindAction(
            PickupDropAction,
            ETriggerEvent::Started,
            this,
            &AAPlayerCharacter::PickupOrDrop
        );
    }
}

void AAPlayerCharacter::StartRunning()
{
    SetRunning(true);
}

void AAPlayerCharacter::StopRunning()
{
    SetRunning(false);
}

void AAPlayerCharacter::SetRunning(const bool bNewIsRunning)
{
    bIsRunning = bNewIsRunning;

    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->MaxWalkSpeed = bIsRunning
            ? WalkingSpeed * RunningSpeedMultiplier
            : WalkingSpeed;
    }

    ApplyCarryingAnimation();

    if (!HasAuthority())
    {
        ServerSetRunning(bNewIsRunning);
    }
    else
    {
        ForceNetUpdate();
    }
}

void AAPlayerCharacter::ServerSetRunning_Implementation(const bool bNewIsRunning)
{
    SetRunning(bNewIsRunning);
}

void AAPlayerCharacter::OnRep_IsRunning()
{
    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->MaxWalkSpeed = bIsRunning
            ? WalkingSpeed * RunningSpeedMultiplier
            : WalkingSpeed;
    }

    ApplyCarryingAnimation();
}


void AAPlayerCharacter::Move(
    const FInputActionValue& Value
)
{
    const FVector2D MovementVector =
        Value.Get<FVector2D>();

    AddMovementInput(
        FVector::ForwardVector,
        MovementVector.Y
    );

    AddMovementInput(
        FVector::RightVector,
        MovementVector.X
    );
}



void AAPlayerCharacter::Interact()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("PlayerCharacter::Interact 호출")
    );

    if (!HasAuthority())
    {
        ServerInteract();
        return;
    }

    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

void AAPlayerCharacter::StopInteract()
{
    if (!HasAuthority())
    {
        ServerStopInteract();
        return;
    }

    if (InteractionComponent)
    {
        InteractionComponent->StopInteract();
    }
}

void AAPlayerCharacter::PickupOrDrop()
{
    if (!HasAuthority())
    {
        ServerPickupOrDrop();
        return;
    }

    if (!ItemHolderComponent
        || !InteractionComponent)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("PickupOrDrop: 필요한 컴포넌트가 없음")
        );

        return;
    }

    if (ItemHolderComponent->GetHeldObject())
    {
        // 일반 탁자를 바라보고 있다면 탁자 배치로 E키 입력을 처리합니다.
        if (InteractionComponent->
            TryPlaceHeldItemOnTable())
        {
            return;
        }

        // 일반 탁자가 없다면 기존처럼 캐릭터 앞 바닥에 내려놓습니다.
        ItemHolderComponent->Release();
        return;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("PickupOrDrop: 현재 들고 있는 객체가 없음")
    );

    InteractionComponent->TryPickup();
}

void AAPlayerCharacter::ServerInteract_Implementation()
{
    Interact();
}

void AAPlayerCharacter::ServerStopInteract_Implementation()
{
    StopInteract();
}

void AAPlayerCharacter::ServerPickupOrDrop_Implementation()
{
    PickupOrDrop();
}

void AAPlayerCharacter::ShowInvalidOrderPopup()
{
    if (!InvalidOrderWidgetComponent)
    {
        return;
    }

    InvalidOrderWidgetComponent->SetVisibility(true);

    FTimerHandle HideTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        HideTimerHandle,
        [this]()
        {
            if (InvalidOrderWidgetComponent)
            {
                InvalidOrderWidgetComponent->SetVisibility(false);
            }
        },
        1.1f,
        false
    );
}
