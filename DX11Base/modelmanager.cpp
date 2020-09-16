#include "pch.h"
#include "modelmanager.h"


std::vector<std::pair<ModelType, std::shared_ptr<Model>>>  ModelManager::m_modelDatas;
std::vector<std::pair<ModelType, const char*>> ModelManager::m_modelPaths =
{
	{ModelType::MODEL_BULLET, "asset\\model\\Bullet.obj"},
	{ModelType::MODEL_PLAYER, "asset\\model\\mutant.fbx"},
	{ModelType::MODEL_SKYBOX, "asset\\model\\Skybox.obj"},
	{ModelType::MODEL_FLOOR, "asset\\model\\Floor.obj"}
};


void ModelManager::GetModel(ModelType type, std::shared_ptr<Model>& pOutModel)
{
	// search for already loaded models
	for (int i = 0; i < m_modelDatas.size(); ++i)
	{
		// found the pointer to the mesh
		if (type == m_modelDatas[i].first)
		{
			// if the shared pointer has released the memory, delete the data and create it again
			if (m_modelDatas[i].second == nullptr)
			{
				m_modelDatas.erase(m_modelDatas.begin() + i);
				break;
			}

			// assign the pointer and return
			pOutModel = m_modelDatas[i].second;
			return;
		}
	}

	// mesh not loaded yet, so load it and return the pointer to it
	pOutModel = Load(type);
}

void ModelManager::LoadModelIntoMemory(ModelType type)
{
	// return if the type is already loaded into memory
	if (IsLoaded(type))
		return;

	// else load
	Load(type);
}

void ModelManager::UnloadAllModel()
{
	for (auto model : m_modelDatas)
	{
		model.second->Unload();
		model.second.reset();
	}

	m_modelDatas.clear();
}

bool ModelManager::IsLoaded(ModelType type)
{
	for (int i = 0; i < m_modelDatas.size(); ++i)
	{
		if (m_modelDatas[i].first == type)
			return true;
	}

	return false;
}

std::shared_ptr<Model> ModelManager::Load(ModelType type)
{
	// load the model type into memory
	for (int i = 0; i < m_modelPaths.size(); ++i)
	{
		if (m_modelPaths[i].first == type)
		{
			m_modelDatas.emplace_back(std::make_pair(type, std::make_shared<Model>()));
			m_modelDatas.back().second->Load(m_modelPaths[i].second);
			return m_modelDatas.back().second;
		}
	}

	return nullptr;
}
