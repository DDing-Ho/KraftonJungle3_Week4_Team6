#include "StaticMeshComponent.h"

#include "ObjManager.h"

void UStaticMeshComponent::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	StaticMesh = InStaticMesh;

	if (StaticMesh)
	{
		int32 NeededMaterialSlots = StaticMesh->GetNumSections();
		Materials.resize(NeededMaterialSlots, nullptr);
		UpdateBounds();
	}
	else
	{
		Materials.clear();
	}
}

FBoxSphereBounds UStaticMeshComponent::GetLocalBounds() const
{
	if (StaticMesh)
	{
		return StaticMesh->LocalBounds;
	}
	return UPrimitiveComponent::GetLocalBounds();
}

FBoxSphereBounds UStaticMeshComponent::CalcBounds(const FMatrix& LocalToWorld) const
{
	return UPrimitiveComponent::CalcBounds(LocalToWorld);
}

/*void UStaticMeshComponent::Serialize(FArchive& Ar)
{
	FString AssetName;
	if (Ar.IsSaving())
	{
		if (StaticMesh)
		{
			AssetName = StaticMesh->GetAssetPathFileName();
		}
		Ar.Serialize("ObjStaticMeshAsset", AssetName);
	}
	else
	{
		Ar.Serialize("ObjStaticMeshAsset", AssetName);
		if (!AssetName.empty())
		{
			SetStaticMesh(FObjManager::LoadObjStaticMesh(AssetName));
		}
	}
}*/
