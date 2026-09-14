// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "APlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UInteractionComponent;
class UItemHolderComponent;
class UWidgetComponent;

UCLASS()
class OVERCOOKED_API AAPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAPlayerCharacter();

    virtual void BeginPlay() override;
    
    UFUNCTION(BlueprintCallable, Category = "Overcooked|UI")
    void ShowInvalidOrderPopup();
    
protected:
    virtual void SetupPlayerInputComponent(
        class UInputComponent* PlayerInputComponent
    ) override;

private:

    // 카메라 Spring Arm
    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Camera",
        meta = (AllowPrivateAccess = "true")
    )
    TObjectPtr<USpringArmComponent> SpringArm;

    // 실제 카메라
    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Camera",
        meta = (AllowPrivateAccess = "true")
    )
    TObjectPtr<UCameraComponent> Camera;

    // 잘못된 주문 팝업
    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Overcooked|UI",
        meta = (AllowPrivateAccess = "true")
    )
    TObjectPtr<UWidgetComponent> InvalidOrderWidgetComponent;

    // Input Mapping Context
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> PlayerMappingContext;

    // Move Input Action
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    void Move(const FInputActionValue& Value);
    void Interact();
    void StopInteract();
    void PickupOrDrop();

    UFUNCTION(Server, Reliable)
    void ServerInteract();

    UFUNCTION(Server, Reliable)
    void ServerStopInteract();

    UFUNCTION(Server, Reliable)
    void ServerPickupOrDrop();

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Interaction",
        meta = (AllowPrivateAccess = "true")
    )
    TObjectPtr<UInteractionComponent> InteractionComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> InteractAction;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Item",
        meta = (AllowPrivateAccess = "true")
    )
    TObjectPtr<UItemHolderComponent> ItemHolderComponent;
    

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PickupDropAction;
};