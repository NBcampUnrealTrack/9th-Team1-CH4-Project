// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionComponent.h"
#include "ItemHolderComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "InputAction.h"




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