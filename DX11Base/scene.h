#pragma once

#include <typeinfo>
#include "pch.h"
#include "gameObject.h"
#include "light.h"
#include "camera.h"
#include "frustumculling.h"


class Scene
{
protected:
	unsigned const int m_renderQueue = 3; 							// 0 == opaque, 1 == transparent, 2 == ui
	std::list<std::shared_ptr<GameObject>>* m_gameObjects;			// list of gameobjects in the scene
	std::shared_ptr<Camera> m_mainCamera = nullptr;					// the main camera for this scene
	bool optimizedThisFrame = false;								// flag to prevent optimizing multiple times per frame

public:
	Scene() {}
	virtual ~Scene() {}
	
	virtual void Init() = 0;

	virtual void Uninit()
	{
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (auto go : m_gameObjects[i])
			{
				go->Uninit();
			}

			m_gameObjects[i].clear();
		}

		delete[] m_gameObjects;
		m_gameObjects = nullptr;
		m_mainCamera = nullptr;

		LightManager::UninitLighting();
	}

	virtual void Update()
	{
		// reenable optimizing for this frame
		optimizedThisFrame = false;

		// update the camera
		m_mainCamera->Update();

		// update all gameobjects in scene
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (auto go : m_gameObjects[i])
			{
				// init if the recently added object hasnt been initialized
				if (!go->m_initialized)
					go->Init();

				go->Update();
			}

			// delete gameobjects flagged by destroy
			m_gameObjects[i].remove_if([](std::shared_ptr<GameObject> go) { return go->Destroy(); });
		}
	}

	virtual void Draw(UINT renderPass)
	{
		// update the view and projection matrix
		m_mainCamera->Draw(renderPass);

		// optimize for rendering
		//OptimizeListForRendering();

		// draw
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (auto go : m_gameObjects[i])
			{
				// init if the object hasnt been initialized
				if (!go->m_initialized)
					go->Init();

				if(go->m_draw)
					go->Draw(renderPass);
			}
		}
	}

	virtual void Draw(const std::shared_ptr<class Shader>& shader, UINT renderPass)
	{
		// update the view and projection matrix
		m_mainCamera->Draw(renderPass);

		// optimize for rendering
		OptimizeListForRendering();

		// draw with the given shader
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (auto go : m_gameObjects[i])
			{
				// init if the object hasnt been initialized
				if (!go->m_initialized)
					go->Init();

				if (go->m_draw)
					go->Draw(shader, renderPass);
			}
		}
	}

	template<typename T>
	std::shared_ptr<T> AddGameObject(const int renderQueue)
	{
		// check for invalid layer
		if (renderQueue < 0 || renderQueue > m_renderQueue - 1)
			return nullptr;

		std::shared_ptr<T> go = std::make_shared<T>();
		m_gameObjects[renderQueue].emplace_back(go);
		go->Awake();

		return go;
	}

	template<typename T>
	std::vector<std::shared_ptr<T>> GetGameObjects(const int renderQueue)
	{
		auto objects = std::vector<std::shared_ptr<T>>();

		// check for invalid layer
		if (renderQueue < 0 || renderQueue > m_renderQueue - 1)
			return objects;

		// search for every gameobject of the given type in the layer
		for (auto go : m_gameObjects[renderQueue])
		{
			if (typeid(*go) == typeid(T))
			{
				objects.emplace_back(std::static_pointer_cast<T>(go));
			}
		}

		return objects;
	}

	std::shared_ptr<Camera> GetMainCamera()
	{
		return m_mainCamera;
	}

	void OptimizeListForRendering()
	{
		if (optimizedThisFrame)
			return;

		// opaque == z sort front to back
		m_gameObjects[0].sort([&](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b)
		{
			dx::XMVECTOR viewPosA = dx::XMVector3TransformCoord(a->GetPosition(), m_mainCamera->GetViewMatrix());
			dx::XMVECTOR viewPosB = dx::XMVector3TransformCoord(b->GetPosition(), m_mainCamera->GetViewMatrix());
			return dx::XMVectorGetZ(viewPosA) < dx::XMVectorGetZ(viewPosB);
		});

		// transparent == z sort back to front
		m_gameObjects[1].sort([&](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b)
		{
			dx::XMVECTOR viewPosA = dx::XMVector3TransformCoord(a->GetPosition(), m_mainCamera->GetViewMatrix());
			dx::XMVECTOR viewPosB = dx::XMVector3TransformCoord(b->GetPosition(), m_mainCamera->GetViewMatrix());
			return dx::XMVectorGetZ(viewPosA) > dx::XMVectorGetZ(viewPosB);
		});

		// frustum culling, ignore UI layer
		for (int i = 0; i < m_renderQueue - 1; ++i)
		{
			for (const auto& go : m_gameObjects[i])
			{
				if(go->m_enableFrustumCulling)
					go->m_draw = FrustumCulling::CheckPoint(go->GetPosition());
			}
		}

		optimizedThisFrame = true;
	}
};
