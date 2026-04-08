#pragma once

#include "CoreMinimal.h"
#include "ObjectTypes.h"

class UClass;
class UObject;

struct ENGINE_API FObjectCreateParams
{
	UClass* Class = nullptr;
	UObject* Outer = nullptr;
	FString Name = "None";
	EObjectFlags Flags = EObjectFlags::None;
	UObject* Template = nullptr;
};

class ENGINE_API FObjectFactory
{
public:
	static UObject* ConstructObject(const FObjectCreateParams& Params);

	static UObject* ConstructObject(
		UClass* InClass,
		UObject* InOuter = nullptr,
		const FString& InName = "None",
		EObjectFlags InFlags = EObjectFlags::None
	);

	template<typename T>
	static T* ConstructObject(
		UObject* InOuter = nullptr,
		const FString& InName = "None",
		EObjectFlags InFlags = EObjectFlags::None
	)
	{
		static_assert(std::is_base_of_v<UObject, T>, "T must derive from UObject");
		return static_cast<T*>(ConstructObject(T::StaticClass(), InOuter, InName, InFlags));
	}

	static uint32 GetLastUUID();
	static void SetLastUUID(uint32 InUUID);

	/**
	 * copy constructor로 생성된 객체에 새 UUID를 발급하고 GUObjectArray에 등록한다.
	 * UObject::PostDuplicate() 에서만 호출된다.
	 */
	static void RegisterDuplicate(UObject* NewObj);

private:
	static uint32 LastUUID;
	static uint32 GenerateUUID();
};
