#pragma once

#include <map>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"

#pragma comment(lib, "assimp.lib")


class Model
{
	friend class CRenderer;

public:
	void Load(const char* fileName);
	void Unload();
	void Update();

private:
	const aiScene* m_scene = nullptr;
	ID3D11Buffer** m_vertexBuffer;
	ID3D11Buffer** m_indexBuffer;

	std::map<std::string, ID3D11ShaderResourceView*> m_texture;
};
