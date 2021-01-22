#pragma once

#include "renderer.h"
#include "scene.h"
#include "pass.h"


struct RenderPass
{
	std::vector<uint8_t> targetOutput;
	Pass pass;
	std::shared_ptr<class Shader> overrideShader;
	bool clearRTV;
	bool clearDepth, clearStencil;
	D3D11_VIEWPORT* viewPort;
	ID3D11DepthStencilView* depthStencilView;
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
	static void AddRenderPass(const RenderPass& renderPass)
	{
		m_renderPasses.push_back(renderPass);
	}

private:
	static class Scene* m_scene;
	static class Scene* m_nextScene;

	static std::vector<RenderPass> m_renderPasses;

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
		CRenderer::UnbindRenderTargetViews();

		m_scene->Init();
	}
};
