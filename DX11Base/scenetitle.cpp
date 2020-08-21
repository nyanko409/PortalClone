#include "pch.h"
#include "gameObject.h"
#include "scenetitle.h"
#include "scenegame.h"
#include "input.h"
#include "manager.h"


void Title::Init()
{
	m_gameObjects = new std::list<GameObject*>[m_renderQueue];
}

void Title::Update()
{
	Scene::Update();

	if (CInput::GetKeyTrigger(DIK_BACKSPACE))
	{
		CManager::SetActiveScene<Game>();
	}
}
