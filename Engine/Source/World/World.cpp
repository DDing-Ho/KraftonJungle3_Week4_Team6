#include "World.h"
#include "Object/Class.h"  
#include "Scene/Level.h"
#include "Object/ObjectFactory.h"
#include "Component/CameraComponent.h"
#include "Camera/Camera.h"
#include "Serializer/SceneSerializer.h"
#include "Core/Paths.h"
#include "Actor/Actor.h"
IMPLEMENT_RTTI(UWorld, UObject)

UWorld::~UWorld()
{
	CleanupWorld();
}

void UWorld::InitializeWorld(float AspectRatio, ID3D11Device* Device)
{
	PersistentLevel = FObjectFactory::ConstructObject<ULevel>(this, "PersistentLevel");
	if (!PersistentLevel)
	{
		return;
	}

	if (!LevelCameraComponent)
	{
		LevelCameraComponent = FObjectFactory::ConstructObject<UCameraComponent>(this, "LevelCamera");
	}
	if (!ActiveCameraComponent)
	{
		ActiveCameraComponent = LevelCameraComponent;
	}
	if (LevelCameraComponent->GetCamera())
	{
		LevelCameraComponent->GetCamera()->SetAspectRatio(AspectRatio);
	}

	if (Device)
	{
		FSceneSerializer::Load(PersistentLevel, FPaths::FromPath(FPaths::LevelDir() / "DefaultLevel.json"), Device);
	}
}

void UWorld::BeginPlay()
{
	if (bBegunPlay) return;  
	bBegunPlay = true;     
	if (PersistentLevel)
	{
		PersistentLevel->BeginPlay();
	}
	for (ULevel* Level : StreamingLevels)
	{
		if (Level) Level->BeginPlay();
	}
}

void UWorld::Tick(float InDeltaTime)
{
	DeltaSeconds = InDeltaTime;
	WorldTime += InDeltaTime;

	if (PersistentLevel)
	{
		PersistentLevel->Tick(InDeltaTime);
	}
	for (ULevel* Level : StreamingLevels)
	{
		if (Level)
		{
			Level->Tick(InDeltaTime);
		}
	}
}

void UWorld::CleanupWorld()
{
	for (ULevel* Level : StreamingLevels)
	{
		if (Level)
		{
			Level->ClearActors();
			Level->MarkPendingKill();
		}
	}
	if (PersistentLevel)
	{
		PersistentLevel->ClearActors();
		PersistentLevel->MarkPendingKill();
		PersistentLevel = nullptr;
	}
	if (LevelCameraComponent)
	{
		LevelCameraComponent->MarkPendingKill();
	}
	if (ActiveCameraComponent == LevelCameraComponent)
	{
		ActiveCameraComponent = nullptr;
	}
	LevelCameraComponent = nullptr;
	WorldTime = 0.f;
	DeltaSeconds = 0.f;
}

void UWorld::FixupReferences(const FDuplicateionContext& Context)
{
	UObject::FixupReferences(Context);

	if (this->ActiveCameraComponent)
	{
		this->ActiveCameraComponent = static_cast<UCameraComponent*>(Context.GetMappedObject(this->ActiveCameraComponent));
	}
}

void UWorld::CopyPropertiesFrom(const UObject* Source)
{
	UObject::CopyPropertiesFrom(Source);
	const UWorld* SourceWorld = static_cast<const UWorld*>(Source);

	// 단순 변수 얕은 복사
	this->WorldTime = SourceWorld->WorldTime;
	this->DeltaSeconds = SourceWorld->DeltaSeconds;
	this->WorldType = SourceWorld->WorldType;
	this->bBegunPlay = SourceWorld->bBegunPlay;

	// 포인터/배열들 우선 얕은 복사 -> DuplicateSubObjects와 Fixup에서 수리 예정.
	this->PersistentLevel = SourceWorld->PersistentLevel;
	this->StreamingLevels = SourceWorld->StreamingLevels;
	this->LevelCameraComponent = SourceWorld->LevelCameraComponent;
	this->ActiveCameraComponent = SourceWorld->ActiveCameraComponent;
}

void UWorld::DuplicateSubObjects(FDuplicateionContext& Context)
{
	UObject::DuplicateSubObjects(Context);

	if (this->PersistentLevel)
	{
		this->PersistentLevel = static_cast<ULevel*>(this->PersistentLevel->Duplicate(Context, this)); // 복사본 주고, PersistentLevel에서도 Duplicate수행하게 해줌.
	}

	TArray<ULevel*> OldStreamingLevels = this->StreamingLevels;
	this->StreamingLevels.clear();

	for (ULevel* OldLevel : OldStreamingLevels)
	{
		if (OldLevel)
		{
			ULevel* NewLevel = static_cast<ULevel*>(OldLevel->Duplicate(Context, this));
			this->StreamingLevels.push_back(NewLevel);
		}
	}

	if(this->LevelCameraComponent)
	{
		this->LevelCameraComponent = static_cast<UCameraComponent*>(Context.GetMappedObject(this->LevelCameraComponent));
	}
}

void UWorld::DestroyActor(AActor* InActor)
{
	if (!InActor || !PersistentLevel) return;


	if (ActiveCameraComponent && ActiveCameraComponent != LevelCameraComponent)
	{
		for (UActorComponent* Component : InActor->GetComponents())
		{
			if (Component == ActiveCameraComponent)
			{
				ActiveCameraComponent = LevelCameraComponent;
				break;
			}
		}
	}

	PersistentLevel->DestroyActor(InActor);
}

ULevel* UWorld::LoadStreamingLevel(const FString& LevelName, ID3D11Device* Device)
{
	// 이미 로드됐는지 확인
	if (ULevel* Existing = FindStreamingLevel(LevelName))
	{
		return Existing;
	}
	ULevel* NewLevel = FObjectFactory::ConstructObject<ULevel>(this, LevelName);
	if (!NewLevel) return nullptr;

	if (Device)
	{
		FSceneSerializer::Load(NewLevel, FPaths::FromPath(FPaths::LevelDir() / FPaths::ToPath(LevelName + ".json")), Device);
	}
	StreamingLevels.push_back(NewLevel);

	// 이미 게임 진행 중이면 BeginPlay 호출
	if (bBegunPlay)
	{
		NewLevel->BeginPlay();
	}
	return NewLevel;
}

void UWorld::UnloadStreamingLevel(const FString& LevelName)
{
	auto It = std::find_if(StreamingLevels.begin(), StreamingLevels.end(),
		[&](ULevel* Level) { return Level->GetName() == LevelName; });
	if (It != StreamingLevels.end())
	{
		(*It)->ClearActors();
		(*It)->MarkPendingKill();
		StreamingLevels.erase(It);
	}
}

ULevel* UWorld::FindStreamingLevel(const FString& LevelName) const
{
	for (ULevel* Level : StreamingLevels)
	{
		if (Level && Level->GetName() == LevelName)
		{
			return Level;
		}
	}
	return nullptr;
}

TArray<AActor*> UWorld::GetAllActors() const
{
	TArray<AActor*> AllActors;
	if (PersistentLevel)
	{
		const auto& PersistentActors = PersistentLevel->GetActors();
		AllActors.insert(AllActors.end(), PersistentActors.begin(), PersistentActors.end());
	}
	for (ULevel* Level : StreamingLevels)
	{
		if (Level)
		{
			const auto& LevelActors = Level->GetActors();
			AllActors.insert(AllActors.end(), LevelActors.begin(), LevelActors.end());
		}
	}
	return AllActors;
}

const TArray<AActor*>& UWorld::GetActors() const
{
	static TArray<AActor*> EmptyArray;
	if (PersistentLevel)
	{
		return PersistentLevel->GetActors();
	}
	return EmptyArray;
}

void UWorld::SetActiveCameraComponent(UCameraComponent* InCamera)
{
	ActiveCameraComponent = InCamera ? InCamera : LevelCameraComponent;
}

UCameraComponent* UWorld::GetActiveCameraComponent() const
{
	return ActiveCameraComponent ? ActiveCameraComponent.Get() : LevelCameraComponent;
}

FCamera* UWorld::GetCamera() const
{
	UCameraComponent* Cam = GetActiveCameraComponent();
	return Cam ? Cam->GetCamera() : nullptr;
}
