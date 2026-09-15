// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Interaction/InteractionComponent.h"
#include "../Item/ItemHolderComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

#include "InputAction.h"
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
}


void AAPlayerCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent
)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(
            MoveAction,
            ETriggerEvent::Triggered,
            this,
            &AAPlayerCharacter::Move
        );

        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Started,
            this,
            &AAPlayerCharacter::Interact
        );

        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Completed,
            this,
            &AAPlayerCharacter::StopInteract
        );

        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Canceled,
            this,
            &AAPlayerCharacter::StopInteract
        );

        EnhancedInputComponent->BindAction(
            PickupDropAction,
            ETriggerEvent::Started,
            this,
            &AAPlayerCharacter::PickupOrDrop
        );
    }
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
