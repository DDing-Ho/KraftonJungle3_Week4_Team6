#include "GameViewportClient.h"

#include "Core/Engine.h"
#include "Input/InputManager.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "World/World.h"
#include "Scene/Level.h"
#include "Component/CameraComponent.h"
#include "Math/Frustum.h"
#include "Core/ShowFlags.h"

void FGameViewportClient::Attach(FEngine* Engine, FRenderer* Renderer)
{
	if (Renderer)
	{
		// Renderer->ClearViewportCallbacks();
	}
}

void FGameViewportClient::Detach(FEngine* Engine, FRenderer* Renderer)
{
	if (Renderer)
	{
		// Renderer->ClearViewportCallbacks();
	}
}

void FGameViewportClient::Tick(FEngine* Engine, float DeltaTime)
{
	IViewportClient::Tick(Engine, DeltaTime);
}

void FGameViewportClient::HandleMessage(FEngine* Engine, HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
	if (!Engine) return;

	FInputManager* InputManager = Engine->GetInputManager();
	if (InputManager)
	{
		InputManager->ProcessMessage(Hwnd, Msg, WParam, LParam);
	}
}

void FGameViewportClient::BuildRenderCommands(FEngine* Engine, ULevel* Level, const FFrustum& Frustum,
	const FShowFlags& Flags, const FVector& CameraPosition, FRenderCommandQueue& OutQueue)
{
	IViewportClient::BuildRenderCommands(Engine, Level, Frustum, Flags, CameraPosition, OutQueue);
}

void FGameViewportClient::Render(FEngine* Engine, FRenderer* Renderer)
{
	if (!Engine || !Renderer) return;

	ULevel* Level = ResolveLevel(Engine);
	if (!Level) return;

	UWorld* ActiveWorld = ResolveWorld(Engine);
	if (!ActiveWorld) return;

	UCameraComponent* ActiveCamera = ActiveWorld->GetActiveCameraComponent();
	if (!ActiveCamera) return;

	FRenderCommandQueue Queue;
	Queue.Reserve(Renderer->GetPrevCommandCount());
	Queue.ViewMatrix = ActiveCamera->GetViewMatrix();
	Queue.ProjectionMatrix = ActiveCamera->GetProjectionMatrix();

	FFrustum Frustum;
	Frustum.ExtractFromVP(Queue.ViewMatrix * Queue.ProjectionMatrix);

	const FVector CameraPosition = Queue.ViewMatrix.GetInverse().GetTranslation();

	FShowFlags GameShowFlags;
	BuildRenderCommands(Engine, Level, Frustum, GameShowFlags, CameraPosition, Queue);

	Renderer->SubmitCommands(Queue);
	Renderer->ExecuteCommands();
}
