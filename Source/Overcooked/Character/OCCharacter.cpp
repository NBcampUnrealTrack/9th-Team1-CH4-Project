#include "OCCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "../interaction/InteractionComponent.h"
#include "../item/ItemHolderComponent.h"


AOCCharacter::AOCCharacter()
{
    bReplicates = true;
    SetReplicateMovement(true);

    SetNetUpdateFrequency(60.0f);
    SetMinNetUpdateFrequency(20.0f);

    GetCapsuleComponent()->InitCapsuleSize(
        42.0f,
        96.0f
    );

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // --------------------------------------------------
    // Movement
    // --------------------------------------------------

    UCharacterMovementComponent* Movement =
        GetCharacterMovement();

    Movement->bOrientRotationToMovement = true;
    Movement->RotationRate =
        FRotator(0.0f, 640.0f, 0.0f);

    Movement->bConstrainToPlane = true;
    Movement->bSnapToPlaneAtStart = true;

    // --------------------------------------------------
    // Components
    // --------------------------------------------------

    InteractionComponent =
        CreateDefaultSubobject<UInteractionComponent>(
            TEXT("InteractionComponent")
        );

    ItemHolderComponent =
        CreateDefaultSubobject<UItemHolderComponent>(
            TEXT("ItemHolderComponent")
        );

    // --------------------------------------------------
    // Invalid Order UI
    // --------------------------------------------------

    InvalidOrderWidgetComponent =
        CreateDefaultSubobject<UWidgetComponent>(
            TEXT("InvalidOrderWidget")
        );

    InvalidOrderWidgetComponent->SetupAttachment(
        GetRootComponent()
    );

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
}


void AOCCharacter::BeginPlay()
{
    Super::BeginPlay();

    // --------------------------------------------------
    // Enhanced Input
    // --------------------------------------------------

    if (APlayerController* PlayerController =
        Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer =
            PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                LocalPlayer->GetSubsystem<
                    UEnhancedInputLocalPlayerSubsystem>())
            {
                if (PlayerMappingContext)
                {
                    Subsystem->AddMappingContext(
                        PlayerMappingContext,
                        0
                    );
                }
            }
        }
    }
}


void AOCCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(
        PlayerInputComponent
    );

    UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(
            PlayerInputComponent
        );

    if (!EnhancedInputComponent)
    {
        return;
    }

    // --------------------------------------------------
    // Move
    // --------------------------------------------------

    if (MoveAction)
    {
        EnhancedInputComponent->BindAction(
            MoveAction,
            ETriggerEvent::Triggered,
            this,
            &AOCCharacter::Move
        );
    }

    // --------------------------------------------------
    // Interact
    // --------------------------------------------------

    if (InteractAction)
    {
        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Started,
            this,
            &AOCCharacter::Interact
        );

        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Completed,
            this,
            &AOCCharacter::StopInteract
        );

        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Canceled,
            this,
            &AOCCharacter::StopInteract
        );
    }

    // --------------------------------------------------
    // Pickup / Drop
    // --------------------------------------------------

    if (PickupDropAction)
    {
        EnhancedInputComponent->BindAction(
            PickupDropAction,
            ETriggerEvent::Started,
            this,
            &AOCCharacter::PickupOrDrop
        );
    }
}


// ======================================================
// Movement
// ======================================================

void AOCCharacter::Move(
    const FInputActionValue& Value)
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


// ======================================================
// Interaction
// ======================================================

void AOCCharacter::Interact()
{
    if (!HasAuthority())
    {
        ServerInteract();
        return;
    }

    if (!InteractionComponent)
    {
        return;
    }

    InteractionComponent->TryInteract();
}


void AOCCharacter::StopInteract()
{
    if (!HasAuthority())
    {
        ServerStopInteract();
        return;
    }

    if (!InteractionComponent)
    {
        return;
    }

    InteractionComponent->StopInteract();
}


// ======================================================
// Pickup / Drop
// ======================================================

void AOCCharacter::PickupOrDrop()
{
    if (!HasAuthority())
    {
        ServerPickupOrDrop();
        return;
    }

    if (!ItemHolderComponent ||
        !InteractionComponent)
    {
        return;
    }

    // 현재 아이템을 들고 있는 경우
    if (ItemHolderComponent->GetHeldObject())
    {
        // 먼저 테이블 배치를 시도한다.
        if (InteractionComponent->TryPlaceHeldItemOnTable())
        {
            return;
        }

        // 테이블 배치가 아니면 일반 Drop
        ItemHolderComponent->Release();

        return;
    }

    // 아무것도 들고 있지 않으면 Pickup
    InteractionComponent->TryPickup();
}


// ======================================================
// Server RPC
// ======================================================

void AOCCharacter::ServerInteract_Implementation()
{
    Interact();
}


void AOCCharacter::ServerStopInteract_Implementation()
{
    StopInteract();
}


void AOCCharacter::ServerPickupOrDrop_Implementation()
{
    PickupOrDrop();
}


// ======================================================
// Invalid Order Popup
// ======================================================

void AOCCharacter::ShowInvalidOrderPopup()
{
    if (!InvalidOrderWidgetComponent)
    {
        return;
    }

    InvalidOrderWidgetComponent->SetVisibility(true);

    FTimerHandle TimerHandle;

    GetWorldTimerManager().SetTimer(
        TimerHandle,
        [this]()
        {
            if (IsValid(InvalidOrderWidgetComponent))
            {
                InvalidOrderWidgetComponent->SetVisibility(
                    false
                );
            }
        },
        1.1f,
        false
    );
}