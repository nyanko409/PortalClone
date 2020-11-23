#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "modelmanager.h"
#include "scenetitle.h"
#include "scenegame.h"


Scene* CManager::m_scene;
Scene* CManager::m_nextScene;
std::vector<Pass> CManager::m_renderPasses = std::vector<Pass>();


void CManager::Init()
{
	CRenderer::Init();
	CInput::Init();
	Audio::Init(GetWindow());

	SetScene<Game>();

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
	m_renderPasses.clear();
	m_scene->Uninit();
	delete m_scene;

	Audio::StopAllAudio();
	Audio::Uninit();
	CInput::Uninit();
	ModelManager::UnloadAllModel();
	CRenderer::Uninit();
}

void CManager::Update()
{
	ChangeScene();

	CInput::Update();
	Audio::Update();
	m_scene->Update();
}

void CManager::Draw()
{
	// prepare for imgui draw
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// render pass 1 outputs it to the screen ie bound to the back buffer
	//std::vector<uint8_t> targetOutput = {1,2};
	//CRenderer::Begin(targetOutput, true);
	//m_scene->Draw(1);
	//
	//targetOutput = { 1 };
	//CRenderer::Begin(targetOutput, false);
	//m_scene->Draw(10);

	for (int i = 0; i < m_renderPasses.size(); ++i)
	{
		CRenderer::Begin(m_renderPasses[i].targetOutput, m_renderPasses[i].clearPrevBuffer);
		m_scene->Draw(m_renderPasses[i].passId);
	}

	// render imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	CRenderer::End();
}

Scene* CManager::GetActiveScene()
{
	return m_scene;
}
