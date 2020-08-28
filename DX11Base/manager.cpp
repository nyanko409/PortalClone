#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "scenetitle.h"
#include "scenegame.h"


Scene* CManager::m_scene;


void CManager::Init()
{
	CRenderer::Init();
	CInput::Init();
	Audio::Init(GetWindow());

	SetActiveScene<Game>();

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(CRenderer::GetDevice(), CRenderer::GetDeviceContext());
}

void CManager::Uninit()
{
	m_scene->Uninit();
	delete m_scene;

	Audio::Uninit();
	CInput::Uninit();
	ModelManager::UnloadAllModel();
	CRenderer::Uninit();
}

void CManager::Update()
{
	CInput::Update();
	Audio::Update();
	m_scene->Update();
}

void CManager::Draw()
{
	CRenderer::Begin();

	// prepare for imgui draw
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// draw the active scene
	m_scene->Draw();

	// render imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	CRenderer::End();
}

Scene* CManager::GetActiveScene()
{
	return m_scene;
}
