#include "pch.h"
#include "gameObject.h"
#include "scenegame.h"
#include "scenetitle.h"
#include "manager.h"
#include "input.h"
#include "fpscamera.h"
#include "field.h"
#include "model.h"
#include "player.h"
#include "polygon.h"
#include "skybox.h"


void Game::Init()
{
	m_gameObjects = new std::list<GameObject*>[m_renderQueue];

	AddGameObject<FPSCamera>(0);
	AddGameObject<Skybox>(0);
	AddGameObject<Field>(0);
	AddGameObject<Player>(1);
	AddGameObject<CPolygon>(2);

	InitGameObjects();
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
		CManager::SetActiveScene<Title>();
	}
}
