#pragma once

#include "CoreMinimal.h"
#include "MeshData.h"

class ENGINE_API FObjManager
{
private:
	static TMap<FString, FStaticMesh*> ObjStaticMeshMap;

public:
	static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

private:
	static bool ParseObjFile(const FString& FilePath, FStaticMesh* OutMesh);
};
