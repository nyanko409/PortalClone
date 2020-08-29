#include "pch.h"
#include "gameObject.h"
#include "scenetitle.h"
#include "scenegame.h"
#include "input.h"
#include "manager.h"
#include "fade.h"


void Title::Init()
{
	m_gameObjects = new std::list<GameObject*>[m_renderQueue];

	AddGameObject<Fade>(2)->StartFadeOut(0.005F, CManager::SetScene<Game>);
}

void Title::Update()
{
	Scene::Update();

	if (CInput::GetKeyTrigger(DIK_BACKSPACE))
	{
		CManager::SetScene<Game>();
	}
}
