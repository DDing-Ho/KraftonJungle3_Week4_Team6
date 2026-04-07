#include "ActorComponent.h"
#include "Object/Class.h"
#include "Serializer/Archive.h"
#include "Actor/Actor.h"

IMPLEMENT_RTTI(UActorComponent, UObject)

void UActorComponent::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving()) Ar.Serialize("UUID", UUID);
	else
	{
		if (Ar.Contains("UUID"))
		{
			uint32 SavedUUID = 0;
			Ar.Serialize("UUID", SavedUUID);

			GUUIDToObjectMap.erase(UUID);

			if (auto It = GUUIDToObjectMap.find(SavedUUID); It != GUUIDToObjectMap.end() && It->second != this)
			{
				It->second->UUID = 0;
				GUUIDToObjectMap.erase(It);
			}
			UUID = SavedUUID;
			GUUIDToObjectMap[SavedUUID] = this;
		}
	}
}

void UActorComponent::FixupReferences(const FDuplicateionContext& Context)
{
	UObject::FixupReferences(Context);

	if (this->Owner)
	{
		this->Owner = static_cast<AActor*>(Context.GetMappedObject(this->Owner.Get()));
	}
}

void UActorComponent::CopyPropertiesFrom(const UObject* Source)
{
	UObject::CopyPropertiesFrom(Source);

	const UActorComponent* SourceComp = static_cast<const UActorComponent*>(Source);

	this->bRegistered = SourceComp->bRegistered;
	this->bBegunPlay = SourceComp->bBegunPlay;
	this->bCanEverTick = SourceComp->bCanEverTick;
	this->bTickEnabled = SourceComp->bTickEnabled;

	this->Owner = SourceComp->Owner;
}
