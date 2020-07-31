#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "scenegame.h"
#include "scenetitle.h"


Scene* CManager::m_scene;

void CManager::Init()
{
	CRenderer::Init();
	CInput::Init();

	SetActiveScene<Game>();

	// init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(CRenderer::GetDevice(), CRenderer::GetDeviceContext());
}

void CManager::Uninit()
{
	m_scene->Uninit();
	delete m_scene;

	CInput::Uninit();
	CRenderer::Uninit();
}

void CManager::Update()
{
	CInput::Update();
	m_scene->Update();
}

void CManager::Draw()
{
	CRenderer::Begin();

	//LIGHT light;
	//light.Enable = true;
	//light.Direction = dx::XMFLOAT4(1.0F, -1.0F, 1.0F, 0.0F);
	//dx::XMStoreFloat4(&light.Direction, dx::XMVector4Normalize(dx::XMLoadFloat4(&light.Direction)));
	//
	//light.Ambient = dx::XMFLOAT4(0.1F, 0.1F, 0.1F, 1.0F);
	//light.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	//CRenderer::SetLight(light);

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
