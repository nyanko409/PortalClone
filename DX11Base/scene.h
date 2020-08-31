#pragma once

#include <typeinfo>
#include "pch.h"
#include "gameObject.h"


class Scene
{
protected:
	unsigned const int m_renderQueue = 3;				// 0 == opaque, 1 == transparent, 2 == ui
	std::list<std::shared_ptr<GameObject>>* m_gameObjects;

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
	}

	virtual void Update()
	{
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (auto go : m_gameObjects[i])
			{
				// init if the object hasnt been initialized
				if (!go->m_initialized)
					go->Init();

				go->Update();
			}

			// delete gameobjects flagged by destroy
			m_gameObjects[i].remove_if([](std::shared_ptr<GameObject> go) { return go->Destroy(); });
		}
	}

	virtual void Draw()
	{
		for (int i = 0; i < m_renderQueue; ++i)
			for (auto go : m_gameObjects[i])
			{
				// init if the object hasnt been initialized
				if (!go->m_initialized) 
					go->Init();

				go->Draw();
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
				objects.emplace_back(std::dynamic_pointer_cast<T>(go));
			}
		}

		return objects;
	}
};
