#pragma once

#include <map>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment(lib, "assimp.lib")


struct DeformVertex
{
	aiVector3D position;
	aiVector3D normal;

	int boneNum;
	int boneIndex[4]; // not optimal
	float boneWeight[4];
};

struct Bone
{
	aiMatrix4x4 matrix;
	aiMatrix4x4 animationMatrix;
	aiMatrix4x4 offsetMatrix;
};

class Model
{
	friend class CRenderer;

public:
	void Load(const char* fileName);
	void Unload();
	void Update(int frame, int animationNum);

private:
	static std::shared_ptr<class SkinningCompute> m_skinningCs;

	const aiScene* m_scene = nullptr;
	ID3D11Buffer** m_vertexBuffer;
	ID3D11Buffer** m_indexBuffer;

	std::map<std::string, ID3D11ShaderResourceView*> m_texture;

	std::vector<DeformVertex>* m_deformVertices;
	std::vector<std::pair<std::string, Bone>> m_bones;

	int m_boneNodeIndex;

	// for motion blending
	UINT m_prevAnimIndex = 0, m_curAnimIndex = 0;
	UINT m_prevAnim = 0;
	float m_blendRatio = 0.0F;
	float m_blendSpeed = 0.05F;
	bool m_blending = false;

	void CreateBone(aiNode* node);
	void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix);
};
