#pragma once

#include "Actor.h"

class UStaticMeshComponent;

class ENGINE_API APlaneActor : public AActor
{
public:
	DECLARE_RTTI(APlaneActor, AActor)

	void PostSpawnInitialize() override;
	void FixupReferences(const FDuplicateionContext& Context);

protected:
	void CopyPropertiesFrom(const UObject* Source);

private:
	UStaticMeshComponent* PlaneMeshComponent = nullptr;
};