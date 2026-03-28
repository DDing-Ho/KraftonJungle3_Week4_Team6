#pragma once
#include "CoreMinimal.h"
#include "MeshComponent.h"
#include "Serializer/Archive.h"

class UStaticMesh;

class ENGINE_API UStaticMeshComponent : public UMeshComponent
{
public:
	DECLARE_RTTI(UStaticMeshComponent, UMeshComponent)

	void SetStaticMesh(UStaticMesh* InStaticMesh);
	UStaticMesh* GetStaticMesh() const { return StaticMesh; }
	// virtual void Serialize(FArchive& Ar);
	virtual FBoxSphereBounds CalcBounds(const FMatrix& LocalToWorld) const override;
	virtual FBoxSphereBounds GetLocalBounds() const override;

private:
	UStaticMesh* StaticMesh = nullptr;
};