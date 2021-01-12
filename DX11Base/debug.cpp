#include "pch.h"
#include <string>
#include "debug.h"
#include "manager.h"
#include "player.h"
#include "camera.h"
#include "portalmanager.h"


void Debug::Draw()
{
	// player debugging window
	auto activePortal = CManager::GetActiveScene()->GetGameObjects<Player>(0).front()->GetEntrancePortal();
	std::string text = "";
	if (auto portal = activePortal.lock())
		text = portal->GetType() == PortalType::Blue ? "Blue" : "Orange";
	else
		text = "None";

	ImGui::SetNextWindowSize(ImVec2(350, 200));
	ImGui::Begin("Player Debug");
	ImGui::Text("Entrance Portal: %s", text.c_str());
	ImGui::End();

	// camera debugging window
	auto cam = CManager::GetActiveScene()->GetMainCamera();
	int curItem = cam->m_debugCameraNum;
	bool obliqueProj = PortalManager::m_enableObliqueProjection;
	const char* listboxItems[] = { "Main Camera", "Blue Camera", "Orange Camera" };

	ImGui::SetWindowPos(ImVec2(500, 50));
	ImGui::SetNextWindowSize(ImVec2(350, 200));
	ImGui::Begin("Camera Debug");
	ImGui::Text("active camera");
	ImGui::ListBox("", &curItem, listboxItems, IM_ARRAYSIZE(listboxItems), 4);
	ImGui::Spacing();
	ImGui::Checkbox("enable oblique projection", &obliqueProj);
	ImGui::End();

	cam->m_debugCameraNum = curItem;
	PortalManager::m_enableObliqueProjection = obliqueProj;
}
