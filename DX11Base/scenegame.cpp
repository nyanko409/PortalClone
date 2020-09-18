#include "pch.h"
#include "gameObject.h"
#include "scenegame.h"
#include "scenetitle.h"
#include "manager.h"
#include "input.h"
#include "topdowncamera.h"
#include "field.h"
#include "model.h"
#include "player.h"
#include "polygon.h"
#include "skybox.h"
#include "enemyspawner.h"
#include "fade.h"
#include "reloadui.h"
#include "terrain.h"


void Game::Init()
{
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];

	AddGameObject<TopDownCamera>(0);
	//AddGameObject<EnemySpawner>(0);
	AddGameObject<Player>(0);
	AddGameObject<Skybox>(0);
	AddGameObject<Terrain>(0);
	AddGameObject<ReloadUI>(2);
	AddGameObject<Fade>(2)->StartFadeOut(0.005F);
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
