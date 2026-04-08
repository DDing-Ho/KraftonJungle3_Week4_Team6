#include "PlayerStart.h"

#include "Asset/ObjManager.h"
#include "Component/StaticMeshComponent.h"
#include "Core/Paths.h"
#include "Object/ObjectFactory.h"
#include "Object/Class.h"

IMPLEMENT_RTTI(APlayerStart, AActor)

void APlayerStart::PostSpawnInitialize()
{
	UStaticMesh* Mesh = FObjManager::LoadModelStaticMeshAsset(FPaths::FromPath(FPaths::MeshDir() / "PrimitiveBox.Model"));

	MeshComponent = FObjectFactory::ConstructObject<UStaticMeshComponent>(this, "StaticMeshComponent");
	MeshComponent->SetStaticMesh(Mesh);

	AddOwnedComponent(MeshComponent);

	AActor::PostSpawnInitialize();
}
