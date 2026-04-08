#include "PIEToolbarWindow.h"
#include "imgui.h"
#include "EditorEngine.h"

void FPIEToolbarWindow::RenderInMenuBar(FEditorEngine* Engine)
{
	if (!Engine) return;

	const bool bIsPIE = Engine->IsPIEActive();
	const char* Label    = bIsPIE ? "  Stop  " : "  Play  ";
	const float BtnWidth = ImGui::CalcTextSize(Label).x + ImGui::GetStyle().FramePadding.x * 2.0f;

	// 메뉴바 너비 중앙에 커서를 이동한다.
	const float MenuBarWidth = ImGui::GetWindowWidth();
	ImGui::SetCursorPosX((MenuBarWidth - BtnWidth) * 0.5f);

	if (!bIsPIE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.55f, 0.10f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.75f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.05f, 0.35f, 0.05f, 1.0f));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.65f, 0.10f, 0.10f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.45f, 0.05f, 0.05f, 1.0f));
	}

	if (ImGui::Button(Label))
	{
		if (bIsPIE) Engine->StopPIE();
		else        Engine->StartPIE();
	}

	ImGui::PopStyleColor(3);
}
