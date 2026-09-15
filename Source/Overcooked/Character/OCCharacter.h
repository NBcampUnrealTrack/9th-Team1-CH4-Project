#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "OCCharacter.generated.h"


class UWidgetComponent;
class UInputMappingContext;
class UInputAction;
class UInteractionComponent;
class UItemHolderComponent;

UCLASS(Blueprintable)
class OVERCOOKED_API AOCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AOCCharacter();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Overcooked|UI")
	void ShowInvalidOrderPopup();

protected:
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent
	) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Overcooked|UI")
	TObjectPtr<UWidgetComponent> InvalidOrderWidgetComponent;

private:
	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> PickupDropAction;

	// Interaction
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Interaction",
		meta = (AllowPrivateAccess = "true")
	)
	TObjectPtr<UInteractionComponent> InteractionComponent;

	// Item
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Item",
		meta = (AllowPrivateAccess = "true")
	)
	TObjectPtr<UItemHolderComponent> ItemHolderComponent;

	// Input Functions
	void Move(const FInputActionValue& Value);
	void Interact();
	void StopInteract();
	void PickupOrDrop();

	// Server RPC
	UFUNCTION(Server, Reliable)
	void ServerInteract();

	UFUNCTION(Server, Reliable)
	void ServerStopInteract();

	UFUNCTION(Server, Reliable)
	void ServerPickupOrDrop();
};