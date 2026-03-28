#pragma once
#include "Component/PrimitiveComponent.h"

class FMaterial;

class ENGINE_API UMeshComponent : public UPrimitiveComponent
{
public:
	DECLARE_RTTI(UMeshComponent, UPrimitiveComponent)

	void SetMaterial(int32 Index, FMaterial* InMaterial);
	FMaterial* GetMaterial(int32 Index) const;
	int32 GetNumMaterials() const { return Materials.size(); }

protected:
	TArray<FMaterial*> Materials;
};