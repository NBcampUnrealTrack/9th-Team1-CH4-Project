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
class UStaticMeshComponent;
class UAnimInstance;
class UAnimSequence;

UCLASS()
class OVERCOOKED_API AAPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    // 애니메이션 블루프린트가 현재 썰기 상태를 조회합니다.
    UFUNCTION(BlueprintPure, Category = "Interaction|Chopping")
    bool IsChopping() const;

    // 서버의 도마 상호작용만 썰기 상태를 변경합니다.
    void SetIsChopping(bool bNewIsChopping);

    // AnimBP의 썰기 시작 Notify에서 호출해 칼을 표시합니다.
    UFUNCTION(BlueprintCallable, Category = "Interaction|Chopping")
    void ShowChoppingKnife();

    // AnimBP의 썰기 종료 Notify에서 호출해 칼을 숨깁니다.
    UFUNCTION(BlueprintCallable, Category = "Interaction|Chopping")
    void HideChoppingKnife();
    
    UFUNCTION(BlueprintCallable, Category = "Overcooked|UI")
    void ShowInvalidOrderPopup();

    UFUNCTION(BlueprintPure, Category = "Item")
    bool IsCarryingItem() const { return bIsCarryingItem; } //캐릭터.들기

    void SetIsCarryingItem(bool bNewIsCarryingItem); //캐릭터.들기
    
protected:
    virtual void SetupPlayerInputComponent(
        class UInputComponent* PlayerInputComponent
    ) override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

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
    void StartRunning(); //캐릭터.달리기
    void StopRunning(); //캐릭터.달리기
    void SetRunning(bool bNewIsRunning); //캐릭터.달리기
    void Interact();
    void StopInteract();
    void PickupOrDrop();

    UFUNCTION(Server, Reliable)
    void ServerInteract();

    UFUNCTION(Server, Reliable)
    void ServerStopInteract();

    UFUNCTION(Server, Reliable)
    void ServerPickupOrDrop();

    UFUNCTION(Server, Reliable)
    void ServerSetRunning(bool bNewIsRunning); //캐릭터.달리기

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

    // 썰기 중에만 KnifeSocket에 표시하는 장식용 칼 메시입니다.
    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Interaction|Chopping",
        meta = (AllowPrivateAccess = "true")
    )
    TObjectPtr<UStaticMeshComponent> ChoppingKnifeMesh;
    

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UInputAction> PickupDropAction;

    // 서버가 결정한 썰기 상태를 모든 클라이언트의 AnimBP에 전달합니다.
    UPROPERTY(
        Replicated,
        VisibleInstanceOnly,
        BlueprintReadOnly,
        Category = "Interaction|Chopping",
        meta = (AllowPrivateAccess = "true")
    )
    bool bIsChopping = false;

    UFUNCTION()
    void OnRep_IsCarryingItem(); //캐릭터.들기

    void ApplyCarryingAnimation(); //캐릭터.애니메이션

    UPROPERTY(
        ReplicatedUsing = OnRep_IsCarryingItem,
        VisibleInstanceOnly,
        BlueprintReadOnly,
        Category = "Item",
        meta = (AllowPrivateAccess = "true")
    )
    bool bIsCarryingItem = false;

    UFUNCTION()
    void OnRep_IsRunning(); //캐릭터.달리기

    UPROPERTY(
        ReplicatedUsing = OnRep_IsRunning,
        VisibleInstanceOnly,
        BlueprintReadOnly,
        Category = "Movement",
        meta = (AllowPrivateAccess = "true")
    )
    bool bIsRunning = false;

    UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "1.0"))
    float RunningSpeedMultiplier = 2.0f;

    float WalkingSpeed = 0.0f;

    UPROPERTY()
    TObjectPtr<UAnimSequence> ChefCarryAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> PandaCarryAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> PandaIdleCarryAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> ChefRunAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> ChefRunCarryAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> PandaRunAnimation;

    UPROPERTY()
    TObjectPtr<UAnimSequence> PandaRunCarryAnimation;

    UPROPERTY(Transient)
    TSubclassOf<UAnimInstance> DefaultAnimInstanceClass;

    UPROPERTY(Transient)
    TObjectPtr<UAnimSequence> ActiveOverrideAnimation;
};
