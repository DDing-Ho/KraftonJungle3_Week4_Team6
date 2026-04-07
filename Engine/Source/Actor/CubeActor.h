#pragma once

#include "Actor.h"

class UStaticMeshComponent;

class ENGINE_API ACubeActor : public AActor
{
public:
	DECLARE_RTTI(ACubeActor, AActor)

	void PostSpawnInitialize() override;
	void FixupReferences(const FDuplicateionContext& Context) override;

protected:
	void CopyPropertiesFrom(const UObject* Source) override;

private:
	UStaticMeshComponent*CubeMeshComponent = nullptr;
};