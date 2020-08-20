#pragma once

#include <typeinfo>
#include "pch.h"
#include "gameObject.h"


class Scene
{
protected:
	unsigned const int m_renderQueue = 3;				// 0 == opaque, 1 == transparent, 2 == ui
	std::list<GameObject*>* m_gameObjects;

	void InitGameObjects()
	{
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (GameObject* go : m_gameObjects[i])
				go->Init();
		}
	}

public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init() = 0;

	virtual void Uninit()
	{
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (GameObject* go : m_gameObjects[i])
			{
				go->Uninit();
				delete go;
			}

			m_gameObjects[i].clear();
		}

		delete[] m_gameObjects;
	}

	virtual void Update()
	{
		for (int i = 0; i < m_renderQueue; ++i)
		{
			for (GameObject* go : m_gameObjects[i])
				go->Update();

			// delete gameobjects flagged by destroy
			m_gameObjects[i].remove_if([](GameObject* go) { return go->Destroy(); });
		}
	}

	virtual void Draw()
	{
		for (int i = 0; i < m_renderQueue; ++i)
			for (GameObject* go : m_gameObjects[i])
				go->Draw();
	}

	template<typename T>
	T* AddGameObject(const int layer)
	{
		// check for invalid layer
		if (layer < 0 || layer > m_renderQueue - 1)
			return nullptr;

		T* go = new T();
		m_gameObjects[layer].emplace_back(go);
		go->Awake();
		return go;
	}

	template<typename T>
	std::vector<T*> GetGameObjects(const int layer)
	{
		std::vector<T*> objects = std::vector<T*>();

		// check for invalid layer
		if (layer < 0 || layer > m_renderQueue - 1)
			return objects;

		// search for every gameobject of the given type in the layer
		for (GameObject* go : m_gameObjects[layer])
		{
			if (typeid(*go) == typeid(T))
				objects.emplace_back((T*)go);
		}

		return objects;
	}
};
