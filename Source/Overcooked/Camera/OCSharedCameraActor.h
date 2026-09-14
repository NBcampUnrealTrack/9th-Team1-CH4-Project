#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "OCSharedCameraActor.generated.h"

UCLASS(Blueprintable)
class OVERCOOKED_API AOCSharedCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	AOCSharedCameraActor();
};
