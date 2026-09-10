

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OVERCOOKED_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();

	void TryInteract();	
	void TryPickup();

private:
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float InteractionDistance = 150.0f;
};
