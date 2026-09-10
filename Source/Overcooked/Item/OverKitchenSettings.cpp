#include "OverKitchenSettings.h"
#include "OverPickupItem.h"
#include "Engine/StaticMesh.h"

UStaticMesh* UOverKitchenSettings::FindItemMesh(UClass* ItemClass) const
{
	// 가장 가까운 클래스 설정을 우선하며 BP 하위 클래스는 부모 기본값을 상속합니다.
	for (UClass* Current = ItemClass; Current; Current = Current->GetSuperClass())
	{
		for (const FOverItemMeshDefault& Entry : ItemMeshes)
		{
			if (Entry.ItemClass.ToSoftObjectPath() == FSoftObjectPath(Current))
			{
				return Entry.Mesh.LoadSynchronous();
			}
		}
	}
	return nullptr;
}

UStaticMesh* UOverKitchenSettings::FindChoppedMesh(const UStaticMesh* RawMesh) const
{
	if (RawMesh)
	{
		for (const FOverChoppedMeshMapping& Entry : ChoppedMeshes)
		{
			if (Entry.RawMesh.ToSoftObjectPath() == FSoftObjectPath(RawMesh))
			{
				return Entry.ChoppedMesh.LoadSynchronous();
			}
		}
	}
	return nullptr;
}

bool UOverKitchenSettings::IsTableMesh(const UStaticMesh* Mesh) const
{
	return Mesh && TableMeshes.ContainsByPredicate([Mesh](const TSoftObjectPtr<UStaticMesh>& Entry)
	{
		return Entry.ToSoftObjectPath() == FSoftObjectPath(Mesh);
	});
}
