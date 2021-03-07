#include "pch.h"
#include <string>
#include "debug.h"
#include "manager.h"
#include "input.h"
#include "player.h"
#include "portalmanager.h"
#include "scenetitle.h"


int Debug::cameraNum = 0;
bool Debug::obliqueProjectionEnabled = true;
bool Debug::displayCollider = false;
bool Debug::pauseUpdate = false;


void Debug::Draw()
{
	// return if in title screen
	if (dynamic_cast<Title*>(CManager::GetActiveScene()))
		return;

	// toggle update
	if (CInput::GetKeyTrigger(DIK_P))
		pauseUpdate = !pauseUpdate;

	// generic window
	const char* technique = PortalManager::GetPortalTechnique() == PortalTechnique::Stencil ? "Stencil" : "RenderTexture";

	ImGui::SetNextWindowSize(ImVec2(300, 215));
	ImGui::Begin("Debug");
	ImGui::Text("Pause Update: P Key");
	ImGui::Spacing();

	ImGui::Checkbox("display collider", &displayCollider);
	ImGui::Spacing();

	ImGui::Text("recursion count: %i", PortalManager::GetRecursionNum());
	if (ImGui::Button("Increase"))
		PortalManager::SetRecursionNum(PortalManager::GetRecursionNum() + 1);
	if (ImGui::Button("Decrease"))
		PortalManager::SetRecursionNum(PortalManager::GetRecursionNum() - 1);
	ImGui::Spacing(); ImGui::Spacing();

	ImGui::Text("rendering portals with: %s", technique);
	if (ImGui::Button("RenderTexture"))
		PortalManager::SetPortalTechnique(PortalTechnique::RenderToTexture);
	if (ImGui::Button("Stencil"))
		PortalManager::SetPortalTechnique(PortalTechnique::Stencil);
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
