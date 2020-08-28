#pragma once

#include <memory>
#include "renderer.h"
#include "shader.h"


enum ModelType
{
	MODEL_BULLET, MODEL_PLAYER, MODEL_SKYBOX, MODEL_FLOOR
};

// �}�e���A���\����
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
	ID3D11ShaderResourceView*	Texture;

};

// �`��T�u�Z�b�g�\����
struct SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	MODEL_MATERIAL	Material;
};

// ���f���\����
struct Mesh
{
	VERTEX_3D		*VertexArray;
	unsigned int	VertexNum;

	unsigned int	*IndexArray;
	unsigned int	IndexNum;

	SUBSET			*SubsetArray;
	unsigned int	SubsetNum;
};

class CModel
{
private:
	ID3D11Buffer*	m_VertexBuffer;
	ID3D11Buffer*	m_IndexBuffer;
	SUBSET*	m_SubsetArray;
	unsigned int m_SubsetNum;

	void LoadObj( const char *FileName, Mesh *mesh );
	void LoadMaterial( const char *FileName, MODEL_MATERIAL **MaterialArray, unsigned int *MaterialNum );

	void CalculateModelVectors(Mesh* mesh);
	void CalculateTangentBinormal(const VERTEX_3D& v1, const VERTEX_3D& v2, const VERTEX_3D& v3, dx::XMFLOAT3& tangent, dx::XMFLOAT3& binormal);
	void CalculateNormal(const dx::XMFLOAT3& tangent, const dx::XMFLOAT3& binormal, dx::XMFLOAT3& normal);

	void Load( const char *FileName );
	void Unload();

public:
	CModel() = delete;
	CModel(const char* path) { Load(path); }
	~CModel() { Unload(); }

	void Draw(Shader* shader);
};

static class ModelManager
{
public:
	static void GetModel(ModelType type, std::shared_ptr<CModel>& pOutModel);
	static void LoadModelIntoMemory(ModelType type);
	static void UnloadAllModel();

private:
	static std::vector<std::pair<ModelType, std::shared_ptr<CModel>>> m_modelDatas;
	static std::vector<std::pair<ModelType, const char*>> m_modelPaths;

	static std::shared_ptr<CModel> Load(ModelType type);
	static bool IsLoaded(ModelType type);
};
