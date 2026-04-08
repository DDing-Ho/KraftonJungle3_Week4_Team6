#include "BillboardActor.h"
#include "Actor.h"
#include "Object/Class.h"
#include "Component/BillboardComponent.h"

IMPLEMENT_RTTI(ABillboardActor, AActor)
void ABillboardActor::PostSpawnInitialize()
{
	BillboardComp = FObjectFactory::ConstructObject<UBillboardComponent>(this, "BillboardComponent");
	AddOwnedComponent(BillboardComp);

	AActor::PostSpawnInitialize();
}

void ABillboardActor::FixupReferences(const FDuplicateionContext& Context)
{
	AActor::FixupReferences(Context);
	if (this->BillboardComp)
	{
		this->BillboardComp = static_cast<UBillboardComponent*>(Context.GetMappedObject(this->BillboardComp));
	}
}

void ABillboardActor::CopyPropertiesFrom(const UObject* Source)
{
	AActor::CopyPropertiesFrom(Source);
	const ABillboardActor* SourceActor = static_cast<const ABillboardActor*>(Source);

	this->BillboardComp = SourceActor->BillboardComp;
}