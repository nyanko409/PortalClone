#include "pch.h"
#include "gameObject.h"
#include "scenetitle.h"
#include "scenegame.h"
#include "input.h"
#include "manager.h"
#include "fade.h"


void Title::Init()
{
	m_gameObjects = new std::list<std::shared_ptr<GameObject>>[m_renderQueue];
}

void Title::Update()
{
	Scene::Update();

	if (CInput::GetMouseLeftTrigger())
	{
		AddGameObject<Fade>(2)->StartFadeIn(0.005F, CManager::SetScene<Game>);
	}
}
