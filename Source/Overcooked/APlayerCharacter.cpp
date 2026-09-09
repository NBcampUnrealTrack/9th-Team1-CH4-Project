// Fill out your copyright notice in the Description page of Project Settings.


#include "APlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "InputAction.h"
#include "InputMappingContext.h"


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
    
    InteractionComponent->TryInteract();
}