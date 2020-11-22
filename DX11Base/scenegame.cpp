#include "pch.h"
#include "main.h"
#include "gameObject.h"
#include "scenegame.h"
#include "scenetitle.h"
#include "manager.h"
#include "input.h"
#include "topdowncamera.h"
#include "fpscamera.h"
#include "field.h"
#include "model.h"
#include "player.h"
#include "polygon.h"
#include "skybox.h"
#include "enemy.h"
#include "fade.h"
#include "reloadui.h"
#include "terrain.h"
#include "minimap.h"
#include "light.h"


void Game::Init()
{
	// add the game objects
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	AddGameObject<Enemy>(0);
	AddGameObject<Skybox>(0);
	AddGameObject<Terrain>(0)->CreateTerrain(200);
	AddGameObject<Player>(0);
	AddGameObject<Minimap>(2);
	//AddGameObject<Fade>(2)->StartFadeOut(0.005F);

	// init the main camera for this scene
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();
	m_mainCamera->Init();

	// set the light
	LightManager::SetDirectionalLight(dx::XMFLOAT4(0.5F, -1.0F, 0.0F, 0.0F), dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F), dx::XMFLOAT4(.1F, .1F, .1F, 1.0F));
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
