#pragma once

#include "CoreMinimal.h"
#include "OverPickupItem.h"
#include "OverRiceItem.generated.h"

UCLASS(Blueprintable)
class OVERCOOKED_API AOverRiceItem : public AOverPickupItem
{
	GENERATED_BODY()

public:
	AOverRiceItem();
	virtual bool BuildPreparedIngredient(FOCPreparedIngredient& OutIngredient) const override;
};
