#include "pch.h"
#include <string>
#include "debug.h"
#include "manager.h"
#include "player.h"
#include "camera.h"
#include "portalmanager.h"


int Debug::cameraNum = 0;
bool Debug::obliqueProjectionEnabled = true;
bool Debug::displayCollider = false;


void Debug::Draw()
{
	// generic window
	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("Generic Debug");
	ImGui::Checkbox("display collision", &displayCollider);
	ImGui::End();

	// player debugging window
	auto activePortal = CManager::GetActiveScene()->GetGameObjects<Player>(0).front()->GetEntrancePortal();
	std::string text = "";
	if (auto portal = activePortal.lock())
		text = portal->GetType() == PortalType::Blue ? "Blue" : "Orange";
	else
		text = "None";

	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("Player Debug");
	ImGui::Text("Entrance Portal: %s", text.c_str());
	ImGui::End();

	// camera debugging window
	const char* listboxItems[] = { "Main Camera", "Blue Camera", "Orange Camera" };

	ImGui::SetWindowPos(ImVec2(500, 50));
	ImGui::SetNextWindowSize(ImVec2(250, 200));
	ImGui::Begin("Camera Debug");
	ImGui::Text("active camera");
	ImGui::ListBox("", &cameraNum, listboxItems, IM_ARRAYSIZE(listboxItems), 4);
	ImGui::Spacing();
	ImGui::Checkbox("enable oblique projection", &obliqueProjectionEnabled);
	ImGui::End();
}
