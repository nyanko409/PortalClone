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


void Game::Init()
{
	m_gameObjects = new std::list<GameObject*>[m_renderQueue];

	AddGameObject<TopDownCamera>(0);
	AddGameObject<Player>(0);
	AddGameObject<Skybox>(0);
	AddGameObject<Field>(0);
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
