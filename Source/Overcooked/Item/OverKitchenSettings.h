#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OverKitchenSettings.generated.h"

class AOverPickupItem;
class AOverPlateItem;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FOverItemMeshDefault
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AOverPickupItem> ItemClass;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> Mesh;
};

USTRUCT(BlueprintType)
struct FOverChoppedMeshMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> RawMesh;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UStaticMesh> ChoppedMesh;
};

/** 에셋 연결은 프로젝트 설정 > Game > Kitchen에서 편집합니다. */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Kitchen"))
class OVERCOOKED_API UOverKitchenSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Items")
	TArray<FOverItemMeshDefault> ItemMeshes;

	UPROPERTY(Config, EditAnywhere, Category="Items")
	TArray<FOverChoppedMeshMapping> ChoppedMeshes;

	UPROPERTY(Config, EditAnywhere, Category="Stations")
	TSoftObjectPtr<UStaticMesh> CuttingTableMesh;

	UPROPERTY(Config, EditAnywhere, Category="Stations")
	TArray<TSoftObjectPtr<UStaticMesh>> TableMeshes;

	UPROPERTY(Config, EditAnywhere, Category="Crates")
	TMap<FName, TSoftClassPtr<AOverPickupItem>> CrateClasses;

	// 기존 장식용 접시를 운반 가능한 접시로 바꾸는 맵 연결 설정입니다.
	UPROPERTY(Config, EditAnywhere, Category="Level Setup")
	FName SetupLevel;

	UPROPERTY(Config, EditAnywhere, Category="Level Setup")
	TSoftObjectPtr<UStaticMesh> DecorativePlateMesh;

	UPROPERTY(Config, EditAnywhere, Category="Level Setup")
	TSoftClassPtr<AOverPlateItem> PlateClass;

	UPROPERTY(Config, EditAnywhere, Category="Level Setup", meta=(ClampMin="0.0"))
	float PlateScaleMultiplier = 1.0f;

	// 신규 맵에서는 액터에 ServingTableComponent를 직접 추가할 수 있습니다.
	UPROPERTY(Config, EditAnywhere, Category="Level Setup")
	TArray<FName> ServingActorNames;

	UStaticMesh* FindItemMesh(UClass* ItemClass) const;
	UStaticMesh* FindChoppedMesh(const UStaticMesh* RawMesh) const;
	bool IsTableMesh(const UStaticMesh* Mesh) const;
};
