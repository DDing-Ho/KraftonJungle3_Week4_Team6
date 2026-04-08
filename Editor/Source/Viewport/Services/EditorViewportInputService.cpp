#include "Viewport/Services/EditorViewportInputService.h"

#include "EditorEngine.h"
#include "Viewport/EditorViewportRegistry.h"
#include "Actor/Actor.h"
#include "Camera/Camera.h"
#include "Core/Engine.h"
#include "Debug/EngineLog.h"
#include "Gizmo/Gizmo.h"
#include "imgui.h"
#include "Input/InputManager.h"
#include "Picking/Picker.h"
#include "Scene/Level.h"
#include "Slate/SlateApplication.h"
#include "Viewport/Viewport.h"

void FEditorViewportInputService::TickCameraNavigation(
	FEngine* Engine,
	FEditorEngine* EditorEngine,
	FEditorViewportRegistry& ViewportRegistry,
	const FGizmo& Gizmo)
{
	if (!Engine || !EditorEngine)
	{
		return;
	}

	FSlateApplication* Slate = EditorEngine->GetSlateApplication();
	if (!Slate || Slate->GetFocusedViewportId() == INVALID_VIEWPORT_ID)
	{
		return;
	}

	bool bIsPlaying = EditorEngine->IsPlayingInEditor();
	FInputManager* Input = Engine->GetInputManager();
	bool bRightMouseDown = Input && Input->IsMouseButtonDown(FInputManager::MOUSE_RIGHT);

	if (ImGui::GetCurrentContext() && !bIsPlaying && !bRightMouseDown)
	{
		const ImGuiIO& IO = ImGui::GetIO();
		if (IO.WantCaptureKeyboard || IO.WantCaptureMouse)
		{
			return;
		}
	}

	if (!Input || Gizmo.IsDragging())
	{
		return;
	}

	if (!bIsPlaying && !bRightMouseDown)
	{
		return;
	}

	FViewportEntry* FocusedEntry = ViewportRegistry.FindEntryByViewportID(Slate->GetFocusedViewportId());
	if (!FocusedEntry)
	{
		return;
	}

	const float DeltaX = Input->GetMouseDeltaX();
	const float DeltaY = Input->GetMouseDeltaY();

	if (FocusedEntry->LocalState.ProjectionType == EViewportType::Perspective)
	{
		float Sensitivity = 0.2f;
		if (FCamera* Cam = Engine->GetLevel()->GetCamera())
		{
			Sensitivity = Cam->GetMouseSensitivity();
		}

		FocusedEntry->LocalState.Rotation.Yaw += DeltaX * Sensitivity;
		FocusedEntry->LocalState.Rotation.Pitch -= DeltaY * Sensitivity;
		if (FocusedEntry->LocalState.Rotation.Pitch > 89.0f)
		{
			FocusedEntry->LocalState.Rotation.Pitch = 89.0f;
		}
		if (FocusedEntry->LocalState.Rotation.Pitch < -89.0f)
		{
			FocusedEntry->LocalState.Rotation.Pitch = -89.0f;
		}
		return;
	}

	FVector ViewFwd;
	FVector ViewUp;
	switch (FocusedEntry->LocalState.ProjectionType)
	{
	case EViewportType::OrthoTop:
		ViewFwd = FVector(0, 0, -1);
		ViewUp = FVector(1, 0, 0);
		break;

	case EViewportType::OrthoBottom:
		ViewFwd = FVector(0, 0, 1);
		ViewUp = FVector(1, 0, 0);
		break;

	case EViewportType::OrthoLeft:
		ViewFwd = FVector(0, 1, 0);
		ViewUp = FVector(0, 0, 1);
		break;

	case EViewportType::OrthoRight:
		ViewFwd = FVector(0, -1, 0);
		ViewUp = FVector(0, 0, 1);
		break;

	case EViewportType::OrthoFront:
		ViewFwd = FVector(-1, 0, 0);
		ViewUp = FVector(0, 0, 1);
		break;

	case EViewportType::OrthoBack:
		ViewFwd = FVector(1, 0, 0);
		ViewUp = FVector(0, 0, 1);
		break;

	default:
		return;
	}

	const FVector ViewRight = FVector::CrossProduct(ViewUp, ViewFwd).GetSafeNormal();
	const int32 H = FocusedEntry->Viewport->GetRect().Height;
	if (H <= 0) return;
	float WorldPerPixel = (2.0f * FocusedEntry->LocalState.OrthoZoom) / static_cast<float>(H);
	FocusedEntry->LocalState.OrthoTarget -= ViewRight * DeltaX * WorldPerPixel;
	FocusedEntry->LocalState.OrthoTarget += ViewUp * DeltaY * WorldPerPixel;
}

void FEditorViewportInputService::HandleMessage(
	FEngine* Engine,
	FEditorEngine* EditorEngine,
	HWND Hwnd,
	UINT Msg,
	WPARAM WParam,
	LPARAM LParam,
	FEditorViewportRegistry& ViewportRegistry,
	FPicker& Picker,
	FGizmo& Gizmo,
	const std::function<void()>& OnSelectionChanged)
{
	(void)Hwnd;

	if (!Engine || !EditorEngine)
	{
		return;
	}

	FSlateApplication* Slate = EditorEngine->GetSlateApplication();
	if (!Slate)
	{
		return;
	}

	// 최상단에서 ImGui 점유 확인 (다른 패널 클릭 시 뷰포트 로직 차단 및 포커스 해제)
	if (ImGui::GetCurrentContext())
	{
		const ImGuiIO& IO = ImGui::GetIO();
		if (IO.WantCaptureMouse)
		{
			if (Msg == WM_LBUTTONDOWN || Msg == WM_RBUTTONDOWN || Msg == WM_MBUTTONDOWN)
			{
				// 뷰포트 포커스를 명시적으로 해제 (파란 테두리 제거)
				Slate->ClearFocus();
				return;
			}
			
			if (Msg == WM_LBUTTONUP || Msg == WM_RBUTTONUP || Msg == WM_MBUTTONUP || Msg == WM_MOUSEMOVE)
			{
				return;
			}
		}
	}

	const int32 MouseX = static_cast<int32>(static_cast<short>(LOWORD(LParam)));
	const int32 MouseY = static_cast<int32>(static_cast<short>(HIWORD(LParam)));

	switch (Msg)
	{
	case WM_LBUTTONDOWN:
		Slate->ProcessMouseDown(MouseX, MouseY);
		// PIE 모드일 때 실제 뷰포트 영역(Slate Area) 내부를 클릭했을 때만 마우스 캡처
		if (EditorEngine->IsPlayingInEditor() && Slate->GetIsCoursorInArea())
		{
			if (FInputManager* Input = Engine->GetInputManager())
			{
				Input->SetMouseCapture(true);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		Slate->ProcessMouseDoubleClick(MouseX, MouseY);
		return;
	case WM_RBUTTONDOWN:
		Slate->ProcessMouseDown(MouseX, MouseY);
		// 우클릭 시에도 동일하게 뷰포트 영역 내부인지 확인 후 캡처
		if (EditorEngine->IsPlayingInEditor() && Slate->GetIsCoursorInArea())
		{
			if (FInputManager* Input = Engine->GetInputManager())
			{
				Input->SetMouseCapture(true);
			}
		}
		break;
	case WM_MOUSEMOVE:
		Slate->ProcessMouseMove(MouseX, MouseY);
		break;
	case WM_LBUTTONUP:
		Slate->ProcessMouseUp(MouseX, MouseY);
		break;
	default:
		break;
	}

	if (Msg == WM_MOUSEWHEEL)
	{
		FViewportEntry* FocusedEntry = ViewportRegistry.FindEntryByViewportID(Slate->GetFocusedViewportId());
		if (FocusedEntry && FocusedEntry->LocalState.ProjectionType != EViewportType::Perspective)
		{
			const float WheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(WParam)) / WHEEL_DELTA;
			FocusedEntry->LocalState.OrthoZoom *= (1.0f - WheelDelta * 0.1f);
			if (FocusedEntry->LocalState.OrthoZoom < 1.0f)
			{
				FocusedEntry->LocalState.OrthoZoom = 1.0f;
			}
			if (FocusedEntry->LocalState.OrthoZoom > 10000.0f)
			{
				FocusedEntry->LocalState.OrthoZoom = 10000.0f;
			}
		}
		return;
	}

	if (Slate->IsDraggingSplitter())
	{
		return;
	}

	ULevel* Level = Engine->GetLevel();
	AActor* SelectedActor = EditorEngine->GetSelectedActor();
	if (!Level)
	{
		return;
	}

	const bool bRightMouseDown = Engine->GetInputManager() &&
		Engine->GetInputManager()->IsMouseButtonDown(FInputManager::MOUSE_RIGHT);
	FViewportEntry* Entry = ViewportRegistry.FindEntryByViewportID(Slate->GetFocusedViewportId());

	switch (Msg)
	{
	case WM_KEYDOWN:
	{
		if (Slate->GetFocusedViewportId() == INVALID_VIEWPORT_ID || bRightMouseDown)
		{
			return;
		}

		switch (WParam)
		{
		case 'W':
			Gizmo.SetMode(EGizmoMode::Location);
			return;
		case 'E':
			Gizmo.SetMode(EGizmoMode::Rotation);
			return;
		case 'R':
			Gizmo.SetMode(EGizmoMode::Scale);
			return;
		case 'L':
			Gizmo.ToggleCoordinateSpace();
			UE_LOG("Gizmo Space: %s", Gizmo.GetCoordinateSpace() == EGizmoCoordinateSpace::Local ? "Local" : "World");
			return;
		default:
			return;
		}
	}

	case WM_LBUTTONDOWN:
	{
		FViewport* Viewport = ViewportRegistry.GetViewportById(Slate->GetFocusedViewportId());
		if (!Viewport)
		{
			return;
		}

		const FRect& Rect = Viewport->GetRect();
		ScreenWidth = Rect.Width;
		ScreenHeight = Rect.Height;
		ScreenMouseX = MouseX - Rect.X;
		ScreenMouseY = MouseY - Rect.Y;

		if (SelectedActor && Gizmo.BeginDrag(SelectedActor, Entry, Picker, ScreenMouseX, ScreenMouseY))
		{
			return;
		}

		AActor* PickedActor = Picker.PickActor(Level, Entry, ScreenMouseX, ScreenMouseY);
		EditorEngine->SetSelectedActor(PickedActor);
		if (OnSelectionChanged)
		{
			OnSelectionChanged();
		}
		return;
	}

	case WM_MOUSEMOVE:
	{
		FViewport* Viewport = ViewportRegistry.GetViewportById(Slate->GetHoveredViewportId());
		if (!Viewport)
		{
			Gizmo.ClearHover();
			return;
		}

		FViewportEntry* HoveredEntry = ViewportRegistry.FindEntryByViewportID(Slate->GetHoveredViewportId());
		const FRect& Rect = Viewport->GetRect();
		ScreenWidth = Rect.Width;
		ScreenHeight = Rect.Height;
		ScreenMouseX = MouseX - Rect.X;
		ScreenMouseY = MouseY - Rect.Y;

		if (!Gizmo.IsDragging())
		{
			Gizmo.UpdateHover(SelectedActor, HoveredEntry, Picker, ScreenMouseX, ScreenMouseY);
			return;
		}

		if (Gizmo.UpdateDrag(SelectedActor, HoveredEntry, Picker, ScreenMouseX, ScreenMouseY) && OnSelectionChanged)
		{
			OnSelectionChanged();
		}
		return;
	}

	case WM_LBUTTONUP:
	{
		if (!Gizmo.IsDragging())
		{
			return;
		}

		Gizmo.EndDrag();
		FViewport* Viewport = ViewportRegistry.GetViewportById(Slate->GetHoveredViewportId());
		if (Viewport)
		{
			FViewportEntry* HoveredEntry = ViewportRegistry.FindEntryByViewportID(Slate->GetHoveredViewportId());
			const FRect& Rect = Viewport->GetRect();
			ScreenWidth = Rect.Width;
			ScreenHeight = Rect.Height;
			ScreenMouseX = MouseX - Rect.X;
			ScreenMouseY = MouseY - Rect.Y;
			Gizmo.UpdateHover(SelectedActor, HoveredEntry, Picker, ScreenMouseX, ScreenMouseY);
		}
		else
		{
			Gizmo.ClearHover();
		}

		if (OnSelectionChanged)
		{
			OnSelectionChanged();
		}
		return;
	}

	default:
		return;
	}
}
