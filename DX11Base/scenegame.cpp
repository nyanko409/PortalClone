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
#include "light.h"
#include "sprite.h"
#include "portalmanager.h"


void Game::Init()
{
	Audio::StopAllAudio();
	Audio::PlaySoundA(AUDIO_BGM_GAME, 0.0f);
	Audio::StartFade(AUDIO_BGM_GAME, 0.1f, 2.0f);

	// add the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	auto player = AddGameObject<Player>(0);
	AddGameObject<Stage>(0);
	AddGameObject<Cube>(0);
	AddGameObject<PortalManager>(0);
	
	auto crosshair = AddGameObject<Sprite>(2);
	crosshair->CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 50, false);
	crosshair->SetTexture("asset/texture/Crosshair.png");

	AddGameObject<Fade>(2)->StartFadeOut(0.01f);
	
	// init the main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();
	std::static_pointer_cast<FPSCamera>(m_mainCamera)->SetFollowTarget(player);

	// set the render passes
	PortalManager::SetPortalTechnique(PortalTechnique::Stencil);
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
		AddGameObject<Fade>(2)->StartFadeIn(0.01f, CManager::SetScene<Title>);

		Audio::StopFade();
		Audio::StartFade(AUDIO_BGM_GAME, 0, 1.0f);
	}
}
