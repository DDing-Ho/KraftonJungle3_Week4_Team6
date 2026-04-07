#pragma once
#include "CoreMinimal.h"
#include "Actor/Actor.h"

class UStaticMeshComponent;
class URandomColorComponent;

class ENGINE_API AStaticMeshActor : public AActor
{
public:
	DECLARE_RTTI(AStaticMeshActor, AActor)

	virtual void PostSpawnInitialize() override;

	void FixupReferences(const FDuplicateionContext& Context) override;

protected:
	void CopyPropertiesFrom(const UObject* Source) override;

private:
	UStaticMeshComponent* StaticMeshComp = nullptr;
};

