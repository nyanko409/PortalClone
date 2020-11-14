#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "modelmanager.h"
#include "scenetitle.h"
#include "scenegame.h"


Scene* CManager::m_scene;
Scene* CManager::m_nextScene;


void CManager::Init()
{
	CRenderer::Init();
	CInput::Init();
	Audio::Init(GetWindow());

	SetScene<Game>();

	Audio::PlaySoundA(AUDIO_BGM_GAME);

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

	// draw the active scene to each render texture
	//CRenderer::Begin(2);
	//m_scene->Draw(2);

	// render pass 1 outputs it to the screen
	CRenderer::Begin(1);
	m_scene->Draw(1);

	// render imgui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	CRenderer::End();
}

Scene* CManager::GetActiveScene()
{
	return m_scene;
}
