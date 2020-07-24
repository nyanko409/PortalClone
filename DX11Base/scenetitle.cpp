#include "pch.h"
#include "gameObject.h"
#include "scenetitle.h"
#include "scenegame.h"
#include "input.h"
#include "manager.h"


void Title::Init()
{

}

void Title::Update()
{
	Scene::Update();

	if (CInput::GetKeyTrigger(VK_RETURN))
	{
		CManager::SetActiveScene<Game>();
	}
}
