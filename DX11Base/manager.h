#pragma once

#include "scene.h"


struct Pass
{
	std::vector<uint8_t> targetOutput;
	uint8_t id;
	std::shared_ptr<class Shader> overrideShader;
	bool clearPrevBuffer;
};

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

	// add a render pass to the scene
	static void AddRenderPass(const Pass& renderPass)
	{
		m_renderPasses.push_back(renderPass);
	}

private:
	static class Scene* m_scene;
	static class Scene* m_nextScene;

	static std::vector<Pass> m_renderPasses;

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
		m_renderPasses.clear();

		m_scene->Init();
	}
};
