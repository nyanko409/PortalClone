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


void Title::Init()
{
	// init the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];

	auto title = AddGameObject<Sprite>(2);
	title->CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, false);
	title->SetTexture("asset/texture/Title.png");

	//AddGameObject<Fade>(2)->StartFadeOut(0.005F);

	// main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();

	// set the render passes
	Pass pass = {};
	pass.targetOutput = { 1 };
	pass.clearPrevBuffer = true;
	pass.passId = 1;
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
