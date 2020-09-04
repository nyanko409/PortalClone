#pragma once

#include "model.h"


enum ModelType
{
	MODEL_BULLET, MODEL_PLAYER, MODEL_SKYBOX, MODEL_FLOOR
};

class ModelManager
{
public:
	static void GetModel(ModelType type, std::shared_ptr<Model>& pOutModel);
	static void LoadModelIntoMemory(ModelType type);
	static void UnloadAllModel();

private:
	static std::vector<std::pair<ModelType, std::shared_ptr<Model>>> m_modelDatas;
	static std::vector<std::pair<ModelType, const char*>> m_modelPaths;

	static std::shared_ptr<Model> Load(ModelType type);
	static bool IsLoaded(ModelType type);
};
