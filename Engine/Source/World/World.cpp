#include "World.h"
#include "Object/Class.h"  
#include "Scene/Scene.h"
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
	PersistentLevel = FObjectFactory::ConstructObject<UScene>(this, "PersistentLevel");
	if (!PersistentLevel)
	{
		return;
	}

	if (!SceneCameraComponent)
	{
		SceneCameraComponent = FObjectFactory::ConstructObject<UCameraComponent>(this, "SceneCamera");
	}
	if (!ActiveCameraComponent)
	{
		ActiveCameraComponent = SceneCameraComponent;
	}
	if (SceneCameraComponent->GetCamera())
	{
		SceneCameraComponent->GetCamera()->SetAspectRatio(AspectRatio);
	}

	if (Device)
	{
		FSceneSerializer::Load(PersistentLevel, FPaths::FromPath(FPaths::SceneDir() / "DefaultScene.json"), Device);
	}
}

void UWorld::DuplicateSubObjects()
{
	// PersistentLevel 복사 (sub-object, Outer = World)
	if (PersistentLevel)
	{
		PersistentLevel = static_cast<UScene*>(PersistentLevel->Duplicate(this));
	}

	// StreamingLevels 복사 (sub-objects)
	TArray<UScene*> OrigLevels = StreamingLevels;
	StreamingLevels.clear();
	for (UScene* OrigLevel : OrigLevels)
	{
		if (!OrigLevel) continue;
		if (UScene* LevelCopy = static_cast<UScene*>(OrigLevel->Duplicate(this)))
		{
			StreamingLevels.push_back(LevelCopy);
		}
	}

	// SceneCameraComponent 복사 (sub-object, Outer = World)
	if (SceneCameraComponent)
	{
		SceneCameraComponent = static_cast<UCameraComponent*>(SceneCameraComponent->Duplicate(this));
	}

	// ActiveCameraComponent — 원본이 SceneCamera였으면 복사본으로, 아니면 일단 SceneCamera로
	// TODO: Actor 소속 카메라였을 경우 Duplicate Map으로 정확한 복사본을 연결해야 한다.
	ActiveCameraComponent = SceneCameraComponent;

	// 월드 상태 초기화
	bBegunPlay = false;
	WorldTime = 0.f;
	DeltaSeconds = 0.f;
}

void UWorld::BeginPlay()
{
	if (bBegunPlay) return;  
	bBegunPlay = true;     
	if (PersistentLevel)
	{
		PersistentLevel->BeginPlay();
	}
	for (UScene* Level : StreamingLevels)
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
	for (UScene* Level : StreamingLevels)
	{
		if (Level)
		{
			Level->Tick(InDeltaTime);
		}
	}

}

void UWorld::CleanupWorld()
{
	for (UScene* Level : StreamingLevels)
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
	if (SceneCameraComponent)
	{
		SceneCameraComponent->MarkPendingKill();
	}
	if (ActiveCameraComponent == SceneCameraComponent)
	{
		ActiveCameraComponent = nullptr;
	}
	SceneCameraComponent = nullptr;
	WorldTime = 0.f;
	DeltaSeconds = 0.f;
}

void UWorld::DestroyActor(AActor* InActor)
{
	if (!InActor || !PersistentLevel) return;


	if (ActiveCameraComponent && ActiveCameraComponent != SceneCameraComponent)
	{
		for (UActorComponent* Component : InActor->GetComponents())
		{
			if (Component == ActiveCameraComponent)
			{
				ActiveCameraComponent = SceneCameraComponent;
				break;
			}
		}
	}

	PersistentLevel->DestroyActor(InActor);
}

UScene* UWorld::LoadStreamingLevel(const FString& LevelName, ID3D11Device* Device)
{
	// 이미 로드됐는지 확인
	if (UScene* Existing = FindStreamingLevel(LevelName))
	{
		return Existing;
	}
	UScene* NewLevel = FObjectFactory::ConstructObject<UScene>(this, LevelName);
	if (!NewLevel) return nullptr;

	if (Device)
	{
		FSceneSerializer::Load(NewLevel, FPaths::FromPath(FPaths::SceneDir() / FPaths::ToPath(LevelName + ".json")), Device);
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
		[&](UScene* Level) { return Level->GetName() == LevelName; });
	if (It != StreamingLevels.end())
	{
		(*It)->ClearActors();
		(*It)->MarkPendingKill();
		StreamingLevels.erase(It);
	}
}

UScene* UWorld::FindStreamingLevel(const FString& LevelName) const
{
	for (UScene* Level : StreamingLevels)
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
	for (UScene* Level : StreamingLevels)
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
	ActiveCameraComponent = InCamera ? InCamera : SceneCameraComponent;
}

UCameraComponent* UWorld::GetActiveCameraComponent() const
{
	return ActiveCameraComponent ? ActiveCameraComponent.Get() : SceneCameraComponent;
}

FCamera* UWorld::GetCamera() const
{
	UCameraComponent* Cam = GetActiveCameraComponent();
	return Cam ? Cam->GetCamera() : nullptr;
}
