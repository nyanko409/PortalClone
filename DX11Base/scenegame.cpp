#include "pch.h"
#include "main.h"
#include "gameObject.h"
#include "scenegame.h"
#include "scenetitle.h"
#include "manager.h"
#include "input.h"
#include "fpscamera.h"
#include "player.h"
#include "enemy.h"
#include "fade.h"
#include "terrain.h"
#include "minimap.h"
#include "light.h"
#include "depthfromlightshader.h"
#include "rendertexture.h"


void Game::Init()
{
	// add the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	AddGameObject<Enemy>(0);
	AddGameObject<Terrain>(0)->CreateTerrain(200);
	AddGameObject<Player>(0);

	// init the main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();

	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.01F, -1.0F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));

	// create render textures
	auto lightDepthTexture = std::make_shared<RenderTexture>(2);
	CRenderer::BindRenderTargetView(lightDepthTexture);

	// set the render passes
	Pass pass = {};

	pass = {};
	pass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
	pass.clearPrevBuffer = true;
	pass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
	pass.overrideShader->Init();
	pass.id = 1;
	CManager::AddRenderPass(pass);	
	
	pass = {};
	pass.targetOutput = { 1 };
	pass.clearPrevBuffer = true;
	pass.id = 1;
	CManager::AddRenderPass(pass);
}

void Game::Uninit()
{
	Scene::Uninit();
}

void Game::Update()
{
	Scene::Update();

	if (CInput::GetKeyTrigger(DIK_BACKSPACE))
	{
		CManager::SetScene<Title>();
	}
}
