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


void Game::Init()
{
	m_layerCount = 4;
	m_gameObjects = new std::list<GameObject*>[m_layerCount];

	AddGameObject<FPSCamera>(0);
	AddGameObject<Field>(0);
	AddGameObject<Player>(0);

	AddGameObject<Camera>(1)->SetPerspective(false);
	AddGameObject<CPolygon>(1);
}

void Game::Uninit()
{
	Scene::Uninit();
}

void Game::Update()
{
	Scene::Update();

	if (CInput::GetKeyTrigger(VK_RETURN))
	{
		CManager::SetActiveScene<Title>();
	}
}
