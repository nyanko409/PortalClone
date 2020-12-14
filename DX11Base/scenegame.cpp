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
#include "stage.h"
#include "minimap.h"
#include "light.h"
#include "depthfromlightshader.h"
#include "rendertexture.h"
#include "portal.h"
#include "sprite.h"
#include "bullet.h"


void Game::Init()
{
	// add the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	AddGameObject<Stage>(0);
	AddGameObject<Player>(0);
	AddGameObject<Enemy>(0);


	auto crosshair = AddGameObject<Sprite>(2);
	crosshair->CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, false);
	crosshair->SetTexture("asset/texture/Crosshair.png");

	// init the main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();

	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.5F, -0.5F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));

	// create render textures
	auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, true);
	CRenderer::BindRenderTargetView(lightDepthTexture);

	auto bluePortalTexture = std::make_shared<RenderTexture>(3, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	CRenderer::BindRenderTargetView(bluePortalTexture);
	PortalManager::BindRenderTexture(PortalType::Blue, bluePortalTexture);

	auto orangePortalTexture = std::make_shared<RenderTexture>(4, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	CRenderer::BindRenderTargetView(orangePortalTexture);
	PortalManager::BindRenderTexture(PortalType::Orange, orangePortalTexture);

	// set the render passes
	RenderPass renderPass = {};
	renderPass.targetOutput = { bluePortalTexture->GetRenderTargetViewID() };
	renderPass.clearPrevBuffer = true;
	renderPass.pass = Pass::PortalBlue;
	CManager::AddRenderPass(renderPass);

	renderPass = {};
	renderPass.targetOutput = { orangePortalTexture->GetRenderTargetViewID() };
	renderPass.clearPrevBuffer = true;
	renderPass.pass = Pass::PortalOrange;
	CManager::AddRenderPass(renderPass);

	renderPass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
	renderPass.clearPrevBuffer = true;
	renderPass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
	renderPass.pass = Pass::Default;
	renderPass.viewPort = lightDepthTexture->GetViewPort();
	renderPass.depthStencilView = lightDepthTexture->GetDepthStencilView();
	CManager::AddRenderPass(renderPass);
	
	renderPass = {};
	renderPass.targetOutput = { 1 };
	renderPass.clearPrevBuffer = true;
	renderPass.pass = Pass::Default;
	CManager::AddRenderPass(renderPass);
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
