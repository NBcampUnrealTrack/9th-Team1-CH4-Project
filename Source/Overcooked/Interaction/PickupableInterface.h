#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupableInterface.generated.h"

class AAPlayerCharacter;

UINTERFACE(MinimalAPI)
class UPickupableInterface : public UInterface
{
	GENERATED_BODY()
};

class OVERCOOKED_API IPickupableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void Pickup(AAPlayerCharacter* Player);
};