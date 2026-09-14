#include "OCSharedCameraActor.h"

#include "Camera/CameraComponent.h"

AOCSharedCameraActor::AOCSharedCameraActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetActorEnableCollision(false);

	UCameraComponent* SharedCamera = GetCameraComponent();
	SharedCamera->ProjectionMode = ECameraProjectionMode::Perspective;
	SharedCamera->FieldOfView = 60.0f;
	SharedCamera->bConstrainAspectRatio = true;
	SharedCamera->AspectRatio = 16.0f / 9.0f;

	Tags.AddUnique(TEXT("SharedCamera"));
}
