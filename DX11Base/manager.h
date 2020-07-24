#pragma once

#include "scene.h"


class CManager
{
private:
	static class Scene* m_scene;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	static Scene* GetActiveScene();

	template <class T> 
	static void SetActiveScene()
	{
		if (m_scene)
		{
			m_scene->Uninit();
			delete m_scene;
			m_scene = nullptr;
		}

		m_scene = new T();
		m_scene->Init();
	}
};
