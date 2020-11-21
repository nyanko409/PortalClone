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


void Game::Init()
{
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
	m_mainCamera = std::make_shared<FPSCamera>();
	m_mainCamera->Awake();

	AddGameObject<Enemy>(0);
	AddGameObject<Skybox>(0);
	AddGameObject<Terrain>(0)->CreateTerrain(200);
	AddGameObject<Player>(0);
	//AddGameObject<Minimap>(2);
	//AddGameObject<Fade>(2)->StartFadeOut(0.005F);

	m_mainCamera->Init();
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
