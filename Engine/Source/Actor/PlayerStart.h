#pragma once

#include "Actor.h"

class UStaticMeshComponent;

class ENGINE_API APlayerStart : public AActor
{
public:
	DECLARE_RTTI(APlayerStart, AActor)

	void PostSpawnInitialize() override;

private:
	UStaticMeshComponent* MeshComponent = nullptr;
};
