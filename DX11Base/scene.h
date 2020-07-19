#pragma once

#include <typeinfo>
#include "gameObject.h"
#include "fpscamera.h"
#include "field.h"
#include "model.h"
#include "player.h"
#include "polygon.h"


class Scene
{
protected:
	unsigned int m_layerCount;
	std::list<GameObject*>* m_gameObjects;

public:
	Scene() {}
	virtual ~Scene() {}

	virtual void Init()
	{
		m_layerCount = 4;
		m_gameObjects = new std::list<GameObject*>[m_layerCount];

		AddGameObject<FPSCamera>(0);
		AddGameObject<Field>(0);
		AddGameObject<Player>(0);

		AddGameObject<Camera>(1)->SetPerspective(false);
		AddGameObject<CPolygon>(1);
	}

	virtual void Uninit()
	{
		for (int i = 0; i < m_layerCount; ++i)
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
		for (int i = 0; i < m_layerCount; ++i)
		{
			for (GameObject* go : m_gameObjects[i])
				go->Update();

			// delete gameobjects flagged by destroy
			m_gameObjects[i].remove_if([](GameObject* go) { return go->Destroy(); });
		}
	}

	virtual void Draw()
	{
		for (int i = 0; i < m_layerCount; ++i)
			for (GameObject* go : m_gameObjects[i])
				go->Draw();
	}

	template<typename T>
	T* AddGameObject(const int layer)
	{
		// check for invalid layer
		if (layer < 0 || layer > m_layerCount - 1)
			return nullptr;

		T* go = new T();
		m_gameObjects[layer].emplace_back(go);
		go->Init();
		return go;
	}

	template<typename T>
	std::vector<T*> GetGameObjects(const int layer)
	{
		std::vector<T*> objects = std::vector<T*>();

		// check for invalid layer
		if (layer < 0 || layer > m_layerCount - 1)
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
