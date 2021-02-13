#include "pch.h"
#include "gameObject.h"
#include "scenetitle.h"
#include "scenegame.h"
#include "input.h"
#include "manager.h"
#include "fade.h"
#include "sprite.h"
#include "main.h"
#include "titlecamera.h"
#include "rendertexture.h"
#include "depthfromlightshader.h"
#include "terrain.h"
#include "player.h"


void Title::Init()
{
	// init the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];

	AddGameObject<Terrain>(0)->CreateTerrain(80);
	
	auto player = AddGameObject<Player>(0);
	player->SetPosition(0, 2.3F, 0);
	player->InTitleDisplayMode(true);
	player->EnableFrustumCulling(false);
	player->SetRotation(13, -20, 0);
	
	auto title = AddGameObject<Sprite>(2);
	title->CreatePlaneTopLeft(30, 50, 512, 128, false);
	title->SetTexture("asset/texture/title/Title.png");
	
	title = AddGameObject<Sprite>(2);
	title->CreatePlaneTopLeft(50, 250, 256, 64, false);
	title->SetTexture("asset/texture/title/TextStart.png");
	
	title = AddGameObject<Sprite>(2);
	title->CreatePlaneTopLeft(50, 350, 256, 64, false);
	title->SetTexture("asset/texture/title/TextQuit.png");
	
	AddGameObject<Fade>(2)->StartFadeOut(0.005F);
	
	// main camera for this scene
	m_mainCamera = std::make_shared<TitleCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();
	m_mainCamera->SetPosition(0, 8.0F, -7);
	auto titleCam = std::static_pointer_cast<TitleCamera>(m_mainCamera);
	titleCam->SetFocusTarget(player);
	
	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.5F, -0.5F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));

	// create render textures
	auto lightDepthTexture = std::make_shared<RenderTexture>(2, 2048, 2048, RenderTextureType::Shadowmap);
	CRenderer::BindRenderTargetView(lightDepthTexture);
	
	// set the render passes
	RenderPass renderPass = {};
	renderPass.targetOutput = { lightDepthTexture->GetRenderTargetViewID() };
	renderPass.clearRTV = true;
	renderPass.clearDepth = true;
	renderPass.clearStencil = true;
	renderPass.overrideShader = CRenderer::GetShader<DepthFromLightShader>();
	renderPass.pass = Pass::Default;
	renderPass.depthStencilView = lightDepthTexture->GetDepthStencilView();
	CManager::AddRenderPass(renderPass);

	renderPass = {};
	renderPass.targetOutput = { 1 };
	renderPass.clearDepth = true;
	renderPass.clearStencil = true;
	renderPass.pass = Pass::Default;
	CManager::AddRenderPass(renderPass);
};

void Title::Update()
{
	// update light direction
	static dx::XMFLOAT4 dir = { 0.0F, -0.5F, 0.0F, 0.0F };
	static float x = 0;
	x += 0.5F;
	dir.x = sinf(dx::XMConvertToRadians(x)) * 0.5f;
	dir.z = cosf(dx::XMConvertToRadians(x)) * 0.5f;

	LightManager::SetDirectionalLight(
		dir, dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));
	
	// update the gameobjects in scene
	Scene::Update();

	// switch scene on input
	if (CInput::GetMouseLeftTrigger())
	{
		AddGameObject<Fade>(2)->StartFadeIn(0.005F, CManager::SetScene<Game>);
	}
}
