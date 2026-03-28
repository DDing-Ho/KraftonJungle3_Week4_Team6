#include "MeshComponent.h"

void UMeshComponent::SetMaterial(int32 Index, FMaterial* InMaterial)
{
	if (Index >= 0 && Index < Materials.size())
	{
		Materials[Index] = InMaterial;
	}
}

FMaterial* UMeshComponent::GetMaterial(int32 Index) const
{
	if (Index >= 0 && Index < Materials.size()) return Materials[Index];
	return nullptr;
}
