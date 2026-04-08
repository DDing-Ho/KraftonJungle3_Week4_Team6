#include "PIEToolbarWindow.h"
#include "imgui.h"
#include "IconsFontAwesome6.h"
#include "EditorEngine.h"

void FPIEToolbarWindow::RenderInMenuBar(FEditorEngine* Engine)
{
	if (!Engine) return;

	const bool bPIE    = Engine->IsPIEActive();
	const bool bPaused = Engine->IsPIEPaused();

	// 버튼 너비 계산 (PIE 중에는 Pause/Resume + Stop, 아닐 때는 Play)
	const float BtnW     = 28.0f;
	const float Spacing  = ImGui::GetStyle().ItemSpacing.x;
	const float TotalW   = bPIE ? (BtnW * 2.0f + Spacing) : BtnW;
	const float MenuBarW = ImGui::GetWindowWidth();
	ImGui::SetCursorPosX((MenuBarW - TotalW) * 0.5f);

	if (!bPIE)
	{
		// ▶ Play
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.55f, 0.10f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.75f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.05f, 0.35f, 0.05f, 1.0f));
		if (ImGui::Button(ICON_FA_PLAY, ImVec2(BtnW, 0)))
			Engine->StartPIE();
		ImGui::PopStyleColor(3);
	}
	else
	{
		// ⏸ Pause / ▶ Resume (토글)
		if (!bPaused)
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.65f, 0.55f, 0.05f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.75f, 0.10f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.45f, 0.35f, 0.02f, 1.0f));
			if (ImGui::Button(ICON_FA_PAUSE, ImVec2(BtnW, 0)))
				Engine->PausePIE();
			ImGui::PopStyleColor(3);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.55f, 0.10f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.20f, 0.75f, 0.20f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.05f, 0.35f, 0.05f, 1.0f));
			if (ImGui::Button(ICON_FA_PLAY, ImVec2(BtnW, 0)))
				Engine->ResumePIE();
			ImGui::PopStyleColor(3);
		}

		ImGui::SameLine();

		// ■ Stop
		ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.65f, 0.10f, 0.10f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.45f, 0.05f, 0.05f, 1.0f));
		if (ImGui::Button(ICON_FA_STOP, ImVec2(BtnW, 0)))
			Engine->StopPIE();
		ImGui::PopStyleColor(3);
	}
}
