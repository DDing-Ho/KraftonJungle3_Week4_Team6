#pragma once
#include "CoreMinimal.h"
#include "Actor.h"


class UBillboardComponent;

class ENGINE_API ABillboardActor : public AActor
{
public:
	DECLARE_RTTI(ABillboardActor, AActor)

	virtual void PostSpawnInitialize() override;

	void FixupReferences(const FDuplicateionContext& Context) override;

protected:
	void CopyPropertiesFrom(const UObject* Source) override;

private:
	UBillboardComponent* BillboardComp = nullptr;
};

