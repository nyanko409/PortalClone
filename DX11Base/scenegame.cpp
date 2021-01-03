#include "pch.h"
#include "main.h"
#include "gameObject.h"
#include "scenegame.h"
#include "scenetitle.h"
#include "manager.h"
#include "input.h"
#include "fpscamera.h"
#include "player.h"
#include "cube.h"
#include "fade.h"
#include "stage.h"
#include "minimap.h"
#include "light.h"
#include "depthfromlightshader.h"
#include "rendertexture.h"
#include "portal.h"
#include "sprite.h"
#include "portalmanager.h"


void Game::Init()
{
	// add the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	auto player = AddGameObject<Player>(0);
	AddGameObject<Stage>(0);
	AddGameObject<Cube>(0);
	AddGameObject<PortalManager>(0)->SetPlayer(player);

	auto crosshair = AddGameObject<Sprite>(2);
	crosshair->CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, false);
	crosshair->SetTexture("asset/texture/Crosshair.png");

	uint32_t recursionCount = 0;
	PortalManager::SetRecursionNum(recursionCount);

	// init the main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();
	std::static_pointer_cast<FPSCamera>(m_mainCamera)->SetFollowTarget(player);

	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.5F, -0.5F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));

	// create render textures
	auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, true);
	CRenderer::BindRenderTargetView(lightDepthTexture);

	auto bluePortalTexture = std::make_shared<RenderTexture>(3, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	CRenderer::BindRenderTargetView(bluePortalTexture);

	auto blueTempPortalTexture = std::make_shared<RenderTexture>(4, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	CRenderer::BindRenderTargetView(blueTempPortalTexture);
	PortalManager::BindRenderTexture(PortalType::Blue, bluePortalTexture, blueTempPortalTexture);

	auto orangePortalTexture = std::make_shared<RenderTexture>(5, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	CRenderer::BindRenderTargetView(orangePortalTexture);

	auto orangeTempPortalTexture = std::make_shared<RenderTexture>(6, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	CRenderer::BindRenderTargetView(orangeTempPortalTexture);
	PortalManager::BindRenderTexture(PortalType::Orange, orangePortalTexture, orangeTempPortalTexture);

	// set the render passes
	RenderPass renderPass = {};
	renderPass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
	renderPass.clearRTV = true;
	renderPass.clearDSV = true;
	renderPass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
	renderPass.pass = Pass::Default;
	renderPass.viewPort = lightDepthTexture->GetViewPort();
	renderPass.depthStencilView = lightDepthTexture->GetDepthStencilView();
	CManager::AddRenderPass(renderPass);

	// render to portal texture (blue portal)
	renderPass = {};
	renderPass.targetOutput = { bluePortalTexture->GetRenderTargetViewID(), blueTempPortalTexture->GetRenderTargetViewID() };
	renderPass.clearRTV = true;
	renderPass.clearDSV = true;
	renderPass.pass = Pass::PortalBlue;
	CManager::AddRenderPass(renderPass);

	for (int i = 1; i <= recursionCount; ++i)
	{
		if(i % 2 == 0)
			renderPass.targetOutput = { bluePortalTexture->GetRenderTargetViewID() };
		else
			renderPass.targetOutput = { blueTempPortalTexture->GetRenderTargetViewID() };
	
		CManager::AddRenderPass(renderPass);
	}

	// render to portal texture (orange portal)
	renderPass.targetOutput = { orangePortalTexture->GetRenderTargetViewID(), orangeTempPortalTexture->GetRenderTargetViewID() };
	renderPass.pass = Pass::PortalOrange;
	CManager::AddRenderPass(renderPass);

	for (int i = 1; i <= recursionCount; ++i)
	{
		if (i % 2 == 0)
			renderPass.targetOutput = { orangePortalTexture->GetRenderTargetViewID() };
		else
			renderPass.targetOutput = { orangeTempPortalTexture->GetRenderTargetViewID() };

		CManager::AddRenderPass(renderPass);
	}

	// finally draw everything
	renderPass = {};
	renderPass.targetOutput = { 1 };
	renderPass.clearRTV = true;
	renderPass.clearDSV = true;
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
