#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "shader.h"
#include "model.h"


void Model::Load(const char* fileName)
{
	// load model from path
	const std::string modelPath(fileName);

	m_scene = aiImportFile(fileName, 
		aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded | 
		aiProcess_CalcTangentSpace | aiProcess_Triangulate);

	assert(m_scene);
	
	// create buffers
	m_vertexBuffer = new ID3D11Buffer*[m_scene->mNumMeshes];
	m_indexBuffer = new ID3D11Buffer*[m_scene->mNumMeshes];

	for (unsigned int m = 0; m < m_scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_scene->mMeshes[m];

		// create vertex buffer
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				vertex[v].Position = dx::XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = dx::XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex[v].Binormal = dx::XMFLOAT3(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
				vertex[v].Tangent = dx::XMFLOAT3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
				vertex[v].TexCoord = dx::XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				vertex[v].Diffuse = dx::XMFLOAT4(1, 1, 1, 1);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer[m]);
			delete[] vertex;
		}

		// create index buffer
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
			{
				const aiFace* face = &mesh->mFaces[f];
				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_indexBuffer[m]);
			delete[] index;
		}
	}

	// load textures
	for (unsigned int m = 0; m < m_scene->mNumMaterials; ++m)
	{
		aiString path;
		//m_scene->mMaterials[m]->GetTexture(aiTextureType_NORMALS, 0, &path);
		if (m_scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			// if first char is *, the texture is embedded into the file
			if (path.data[0] == '*')
			{
				if (m_texture[path.data] == NULL)
				{
					ID3D11ShaderResourceView* texture;
					int id = atoi(&path.data[1]);

					D3DX11CreateShaderResourceViewFromMemory(CRenderer::GetDevice(), (const unsigned char*)m_scene->mTextures[id]->pcData,
						m_scene->mTextures[id]->mWidth, NULL, NULL, &texture, NULL);

					m_texture[path.data] = texture;
				}
			}
			else
			{
				if (m_texture[path.data] == NULL)
				{
					// load texture from path
					std::string finalPath = "asset\\model\\texture\\";
					finalPath += path.data;

					ID3D11ShaderResourceView* texture;
					D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(), finalPath.c_str(), NULL, NULL, &texture, NULL);

					m_texture[path.data] = texture;
				}
			}
		}
		else
		{
			m_texture[path.data] = nullptr;
		}
	}
}

void Model::Unload()
{
	for (int i = 0; i < m_scene->mNumMeshes; ++i)
	{
		SAFE_RELEASE(m_vertexBuffer[i]);
		SAFE_RELEASE(m_indexBuffer[i]);
	}

	SAFE_DELETE_ARRAY(m_vertexBuffer);
	SAFE_DELETE_ARRAY(m_indexBuffer);

	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_texture)
	{
		SAFE_RELEASE(pair.second);
	}

	m_texture.clear();

	aiReleaseImport(m_scene);
}

void Model::Update()
{

}

void Model::Draw(const std::shared_ptr<Shader> shader)
{
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1, 1, 1, 1);
	material.Ambient = dx::XMFLOAT4(1, 1, 1, 1);
	shader->SetMaterial(material);

	for (unsigned int m = 0; m < m_scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_scene->mMeshes[m];

		// set texture
		aiMaterial* material = m_scene->mMaterials[mesh->mMaterialIndex];

		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		shader->PS_SetTexture(m_texture[path.data]);

		// set vertex buffer
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer[m], &stride, &offset);

		// set index buffer
		CRenderer::GetDeviceContext()->IASetIndexBuffer(m_indexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// draw
		CRenderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}
