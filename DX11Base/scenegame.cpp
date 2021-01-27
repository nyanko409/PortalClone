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
#include "sprite.h"

#include "portalstencil.h"
#include "portalstencilshader.h"

#include "portalmanager.h"
#include "stencilonlyshader.h"


void Game::Init()
{
	// add the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	auto player = AddGameObject<Player>(0);
	AddGameObject<Stage>(0);
	AddGameObject<Cube>(0);
	AddGameObject<PortalManager>(0);
	
	auto crosshair = AddGameObject<Sprite>(2);
	crosshair->CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, false);
	crosshair->SetTexture("asset/texture/Crosshair.png");
	
	uint32_t recursionCount = 1;
	PortalManager::SetRecursionNum(recursionCount);
	
	// init the main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();
	std::static_pointer_cast<FPSCamera>(m_mainCamera)->SetFollowTarget(player);
	
	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.5F, -0.5F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));
	
	// create lightmap
	auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, true);
	CRenderer::BindRenderTargetView(lightDepthTexture);

	// write to lightmap in the first pass
	RenderPass renderPass = {};
	renderPass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
	renderPass.clearRTV = true;
	renderPass.clearDepth = true;
	renderPass.clearStencil = true;
	renderPass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
	renderPass.pass = Pass::Lightmap;
	renderPass.viewPort = lightDepthTexture->GetViewPort();
	renderPass.depthStencilView = lightDepthTexture->GetDepthStencilView();
	CManager::AddRenderPass(renderPass);

	PortalManager::SetPortalTechnique(PortalTechnique::RenderToTexture);
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
