#include "scene.h"
#include "manager.h"
#include "renderer.h"
#include "input.h"


Scene* CManager::m_scene;

void CManager::Init()
{
	CRenderer::Init();
	CInput::Init();

	m_scene = new Scene();
	m_scene->Init();
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

	LIGHT light;
	light.Enable = true;
	light.Direction = D3DXVECTOR4(1.0F, -1.0F, 1.0F, 0.0F);
	D3DXVec4Normalize(&light.Direction, &light.Direction);
	light.Ambient = D3DXCOLOR(0.1F, 0.1F, 0.1F, 1.0F);
	light.Diffuse = D3DXCOLOR(1.0F, 1.0F, 1.0F, 1.0F);
	CRenderer::SetLight(light);

	m_scene->Draw();

	light.Enable = false;
	CRenderer::SetLight(light);

	CRenderer::End();
}

Scene* CManager::GetActiveScene()
{
	return m_scene;
}
