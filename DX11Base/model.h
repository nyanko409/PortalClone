#pragma once

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"

#pragma comment(lib, "assimp.lib")


class Model
{
public:
	void Load(const char* fileName);
	void Unload();
	void Update();
	void Draw(const std::shared_ptr<class Shader> shader);

private:
	const aiScene* m_scene = nullptr;
	ID3D11Buffer** m_vertexBuffer;
	ID3D11Buffer** m_indexBuffer;
};
