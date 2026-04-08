#include "ActorComponent.h"
#include "Actor/Actor.h"
#include "Object/Class.h"
#include "Serializer/Archive.h"

IMPLEMENT_RTTI(UActorComponent, UObject)

void UActorComponent::DuplicateSubObjects()
{
	// Owner 캐시를 복사된 Outer(= Actor_copy)로 재설정한다.
	Owner = GetTypedOuter<AActor>();

	bBegunPlay = false;
	bRegistered = false;
}

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
