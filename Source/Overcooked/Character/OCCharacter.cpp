#include "OCCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

AOCCharacter::AOCCharacter()
{
	bReplicates = true;
	SetReplicateMovement(true);
	SetNetUpdateFrequency(60.0f);
	SetMinNetUpdateFrequency(20.0f);

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	InvalidOrderWidgetComponent =
		CreateDefaultSubobject<UWidgetComponent>(TEXT("InvalidOrderWidget"));

	InvalidOrderWidgetComponent->SetupAttachment(GetRootComponent());
	InvalidOrderWidgetComponent->SetRelativeLocation(
		FVector(0.0f, 0.0f, 140.0f)
	);
	InvalidOrderWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InvalidOrderWidgetComponent->SetDrawSize(
		FVector2D(160.0f, 160.0f)
	);
	InvalidOrderWidgetComponent->SetVisibility(false);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	Movement->bConstrainToPlane = true;
	Movement->bSnapToPlaneAtStart = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ChefMesh(
		TEXT("/Game/External/Quaternius/UltimateAnimatedCharacters/Chefs/Chef_Male.Chef_Male")
	);

	if (ChefMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMeshAsset(ChefMesh.Object);
		GetMesh()->SetRelativeLocation(
			FVector(0.0f, 0.0f, -96.0f)
		);
		GetMesh()->SetRelativeRotation(
			FRotator(0.0f, -90.0f, 0.0f)
		);
	}
}