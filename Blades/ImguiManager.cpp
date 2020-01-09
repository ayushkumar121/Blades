#include "ImguiManager.h"
#include "imgui/imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsLight();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
