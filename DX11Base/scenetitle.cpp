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

	auto title = AddGameObject<Sprite>(2);
	title->CreatePlaneTopLeft(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	title->SetTexture("asset/texture/title/title.png");
	
	AddGameObject<Fade>(2)->StartFadeOut(0.005F);
	
	// main camera for this scene
	m_mainCamera = std::make_shared<TitleCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();
	m_mainCamera->SetPosition(0, 8.0F, -7);

	// init the render pass
	RenderPass renderPass = {};
	renderPass.targetOutput = { 1 };
	renderPass.clearDepth = true;
	renderPass.clearStencil = true;
	renderPass.pass = Pass::UI;
	CManager::AddRenderPass(renderPass);
};

void Title::Update()
{	
	// update the gameobjects in scene
	Scene::Update();

	// switch scene on input
	if (CInput::GetMouseLeftTrigger())
	{
		AddGameObject<Fade>(2)->StartFadeIn(0.01f, CManager::SetScene<Game>);
	}
}
