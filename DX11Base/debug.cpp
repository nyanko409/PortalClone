#include "pch.h"
#include <string>
#include "debug.h"
#include "manager.h"
#include "input.h"
#include "player.h"
#include "portalmanager.h"


int Debug::cameraNum = 0;
bool Debug::obliqueProjectionEnabled = true;
bool Debug::displayCollider = false;
bool Debug::pauseUpdate = false;


void Debug::Draw()
{
	if (CInput::GetKeyTrigger(DIK_P))
		pauseUpdate = !pauseUpdate;

	// generic window
	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("Debug");
	ImGui::Text("Pause Update: P Key");
	ImGui::Spacing();
	ImGui::Checkbox("display collider", &displayCollider);
	ImGui::End();

	// player debugging window
	auto activePortal = PortalStencilManager::GetPortal(CManager::GetActiveScene()->GetGameObjects<Player>(0).front()->GetEntrancePortal());
	std::string text = "";
	if (activePortal)
		text = activePortal->GetType() == PortalStencilType::Blue ? "Blue" : "Orange";
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
