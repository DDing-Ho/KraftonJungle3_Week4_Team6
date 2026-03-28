#pragma once

#include "CoreMinimal.h"

struct ENGINE_API FStaticVertex
{
	FVector Position;
	FVector4 Color;
	FVector Normal;
	FVector2 UV;
};


struct ENGINE_API FMeshSection
{
	uint32 MaterialIndex;
	uint32 StartIndex;
	uint32 IndexCount;
};
