#include "pch.h"
#include "gameObject.h"
#include "scenetitle.h"
#include "scenegame.h"
#include "input.h"
#include "manager.h"
#include "fade.h"
#include "sprite.h"
#include "main.h"
#include "fpscamera.h"
#include "rendertexture.h"
#include "depthfromlightshader.h"


void Title::Init()
{
	// init the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];

	//auto title = AddGameObject<Sprite>(2);
	//title->CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	//title->SetTexture("asset/texture/Title.png");

	//AddGameObject<Fade>(2)->StartFadeOut(0.005F);

	// main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();

	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.5F, -0.5F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));

	// create render textures
	auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, true);
	CRenderer::BindRenderTargetView(lightDepthTexture);

	// set the render passes
	Pass pass = {};

	pass = {};
	pass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
	pass.clearPrevBuffer = true;
	pass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
	pass.overrideShader->Init();
	pass.id = 1;
	pass.viewPort = lightDepthTexture->GetViewPort();
	pass.depthStencilView = lightDepthTexture->GetDepthStencilView();
	CManager::AddRenderPass(pass);

	// set the render passes
	pass = {};
	pass.targetOutput = { 1 };
	pass.clearPrevBuffer = true;
	pass.id = 1;
	CManager::AddRenderPass(pass);
};

void Title::Update()
{
	Scene::Update();

	if (CInput::GetMouseLeftTrigger())
	{
		AddGameObject<Fade>(2)->StartFadeIn(0.005F, CManager::SetScene<Game>);
	}
}
