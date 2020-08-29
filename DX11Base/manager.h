#pragma once

#include "scene.h"


class CManager
{
public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	// returns the current active scene
	static class Scene* GetActiveScene();

	// set the next scene
	template <class T>
	static void SetScene()
	{
		m_nextScene = new T();
	}

private:
	static class Scene* m_scene;
	static class Scene* m_nextScene;

	// change the scene if the next scene is set
	static void ChangeScene()
	{
		if (!m_nextScene)
			return;

		if (m_scene)
		{
			m_scene->Uninit();
			delete m_scene;
			m_scene = nullptr;
		}

		m_scene = m_nextScene;
		m_nextScene = nullptr;

		m_scene->Init();
	}
};
