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

	// deformation bone array
	m_deformVertices = new std::vector<DeformVertex>[m_scene->mNumMeshes];

	// create bones recursively
	CreateBone(m_scene->mRootNode);
	
	// create buffers
	m_vertexBuffer = new ID3D11Buffer*[m_scene->mNumMeshes];
	m_indexBuffer = new ID3D11Buffer*[m_scene->mNumMeshes];

	// loop for every sub meshes
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

		// create deform vertices
		if (mesh->mNumBones > 0)
		{
			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				DeformVertex deformVertex;
				deformVertex.position = mesh->mVertices[v];
				deformVertex.normal = mesh->mNormals[v];
				deformVertex.boneNum = 0;

				for (unsigned int b = 0; b < 4; ++b)
				{
					deformVertex.boneName[b] = "";
					deformVertex.boneWeight[b] = 0.0F;
				}

				m_deformVertices[m].push_back(deformVertex);
			}
		}

		// create bone data
		for (unsigned int b = 0; b < mesh->mNumBones; ++b)
		{
			aiBone* bone = mesh->mBones[b];

			m_bones[bone->mName.C_Str()].offsetMatrix = bone->mOffsetMatrix;

			for (unsigned int w = 0; w < bone->mNumWeights; ++w)
			{
				aiVertexWeight weight = bone->mWeights[w];
				int num = m_deformVertices[m][weight.mVertexId].boneNum;

				m_deformVertices[m][weight.mVertexId].boneWeight[num] = weight.mWeight;
				m_deformVertices[m][weight.mVertexId].boneName[num] = bone->mName.C_Str();
				m_deformVertices[m][weight.mVertexId].boneNum++;

				assert(m_deformVertices[m][weight.mVertexId].boneNum <= 4);
			}
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
					std::string finalPath = "asset\\model\\";
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

	SAFE_DELETE_ARRAY(m_deformVertices);

	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_texture)
	{
		SAFE_RELEASE(pair.second);
	}

	m_texture.clear();

	aiReleaseImport(m_scene);
}

void Model::Update(int frame, int animationNum)
{
	// return if not needed
	if (!m_scene->HasAnimations() || animationNum >= m_scene->mNumAnimations)
		return;

	m_prevAnimIndex = m_curAnimIndex;
	m_curAnimIndex = animationNum;

	// start blending if prev animation is not the current one
	if (m_prevAnimIndex != m_curAnimIndex)
	{
		if (m_blending)
			m_lerpValue = 1 - m_lerpValue;

		m_blending = true;
		m_prevAnim = m_prevAnimIndex;

	}

	if (m_blending)
	{
		m_lerpValue += 0.01F;
		if (m_lerpValue >= 1.0F)
		{
			m_blending = false;
			m_prevAnim = m_curAnimIndex;
			m_lerpValue = 0.0F;
		}
	}

	// motion blending
	aiAnimation* prevAnimation = m_scene->mAnimations[m_prevAnim];
	aiAnimation* animation = m_scene->mAnimations[m_curAnimIndex];

	for (unsigned int c = 0; c < animation->mNumChannels; ++c)
	{
		aiNodeAnim* nodePrevAnim = prevAnimation->mChannels[c];
		aiNodeAnim* nodeAnim = animation->mChannels[c];

		Bone* bone = &m_bones[nodeAnim->mNodeName.C_Str()];

		int f, fp;
		fp = frame % nodePrevAnim->mNumRotationKeys;
		f = frame % nodeAnim->mNumRotationKeys;
		aiQuaternion rot;
		aiQuaternion::Interpolate(rot, nodePrevAnim->mRotationKeys[fp].mValue, nodeAnim->mRotationKeys[f].mValue, m_lerpValue);

		fp = frame % nodePrevAnim->mNumPositionKeys;
		f = frame % nodeAnim->mNumPositionKeys;
		aiVector3D pos = aiVector3D(
			nodePrevAnim->mPositionKeys[fp].mValue.x * (1 - m_lerpValue),
			nodePrevAnim->mPositionKeys[fp].mValue.y * (1 - m_lerpValue),
			nodePrevAnim->mPositionKeys[fp].mValue.z * (1 - m_lerpValue)) +
			aiVector3D(nodeAnim->mPositionKeys[f].mValue.x * m_lerpValue,
				nodeAnim->mPositionKeys[f].mValue.y * m_lerpValue,
				nodeAnim->mPositionKeys[f].mValue.z * m_lerpValue);

		bone->animationMatrix = aiMatrix4x4(aiVector3D(1.0F, 1.0F, 1.0F), rot, pos);
	}

	// update the bone matrices
	UpdateBoneMatrix(m_scene->mRootNode, aiMatrix4x4());

	// cpu skinning
	for (unsigned int m = 0; m < m_scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = m_scene->mMeshes[m];

		D3D11_MAPPED_SUBRESOURCE ms;
		CRenderer::GetDeviceContext()->Map(m_vertexBuffer[m], 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertices = (VERTEX_3D*)ms.pData;
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			DeformVertex* deformVertex = &m_deformVertices[m][v];

			// support up to 4 bones for single vertex
			aiMatrix4x4 matrix[4];
			aiMatrix4x4 outMatrix;
			matrix[0] = m_bones[deformVertex->boneName[0]].matrix;
			matrix[1] = m_bones[deformVertex->boneName[1]].matrix;
			matrix[2] = m_bones[deformVertex->boneName[2]].matrix;
			matrix[3] = m_bones[deformVertex->boneName[3]].matrix;
			
			// apply weight
			{
				outMatrix.a1 = matrix[0].a1 * deformVertex->boneWeight[0]
					+ matrix[1].a1 * deformVertex->boneWeight[1]
					+ matrix[2].a1 * deformVertex->boneWeight[2]
					+ matrix[3].a1 * deformVertex->boneWeight[3];

				outMatrix.a2 = matrix[0].a2 * deformVertex->boneWeight[0]
					+ matrix[1].a2 * deformVertex->boneWeight[1]
					+ matrix[2].a2 * deformVertex->boneWeight[2]
					+ matrix[3].a2 * deformVertex->boneWeight[3];

				outMatrix.a3 = matrix[0].a3 * deformVertex->boneWeight[0]
					+ matrix[1].a3 * deformVertex->boneWeight[1]
					+ matrix[2].a3 * deformVertex->boneWeight[2]
					+ matrix[3].a3 * deformVertex->boneWeight[3];

				outMatrix.a4 = matrix[0].a4 * deformVertex->boneWeight[0]
					+ matrix[1].a4 * deformVertex->boneWeight[1]
					+ matrix[2].a4 * deformVertex->boneWeight[2]
					+ matrix[3].a4 * deformVertex->boneWeight[3];

				outMatrix.b1 = matrix[0].b1 * deformVertex->boneWeight[0]
					+ matrix[1].b1 * deformVertex->boneWeight[1]
					+ matrix[2].b1 * deformVertex->boneWeight[2]
					+ matrix[3].b1 * deformVertex->boneWeight[3];

				outMatrix.b2 = matrix[0].b2 * deformVertex->boneWeight[0]
					+ matrix[1].b2 * deformVertex->boneWeight[1]
					+ matrix[2].b2 * deformVertex->boneWeight[2]
					+ matrix[3].b2 * deformVertex->boneWeight[3];

				outMatrix.b3 = matrix[0].b3 * deformVertex->boneWeight[0]
					+ matrix[1].b3 * deformVertex->boneWeight[1]
					+ matrix[2].b3 * deformVertex->boneWeight[2]
					+ matrix[3].b3 * deformVertex->boneWeight[3];

				outMatrix.b4 = matrix[0].b4 * deformVertex->boneWeight[0]
					+ matrix[1].b4 * deformVertex->boneWeight[1]
					+ matrix[2].b4 * deformVertex->boneWeight[2]
					+ matrix[3].b4 * deformVertex->boneWeight[3];

				outMatrix.c1 = matrix[0].c1 * deformVertex->boneWeight[0]
					+ matrix[1].c1 * deformVertex->boneWeight[1]
					+ matrix[2].c1 * deformVertex->boneWeight[2]
					+ matrix[3].c1 * deformVertex->boneWeight[3];

				outMatrix.c2 = matrix[0].c2 * deformVertex->boneWeight[0]
					+ matrix[1].c2 * deformVertex->boneWeight[1]
					+ matrix[2].c2 * deformVertex->boneWeight[2]
					+ matrix[3].c2 * deformVertex->boneWeight[3];

				outMatrix.c3 = matrix[0].c3 * deformVertex->boneWeight[0]
					+ matrix[1].c3 * deformVertex->boneWeight[1]
					+ matrix[2].c3 * deformVertex->boneWeight[2]
					+ matrix[3].c3 * deformVertex->boneWeight[3];

				outMatrix.c4 = matrix[0].c4 * deformVertex->boneWeight[0]
					+ matrix[1].c4 * deformVertex->boneWeight[1]
					+ matrix[2].c4 * deformVertex->boneWeight[2]
					+ matrix[3].c4 * deformVertex->boneWeight[3];

				outMatrix.d1 = matrix[0].d1 * deformVertex->boneWeight[0]
					+ matrix[1].d1 * deformVertex->boneWeight[1]
					+ matrix[2].d1 * deformVertex->boneWeight[2]
					+ matrix[3].d1 * deformVertex->boneWeight[3];

				outMatrix.d2 = matrix[0].d2 * deformVertex->boneWeight[0]
					+ matrix[1].d2 * deformVertex->boneWeight[1]
					+ matrix[2].d2 * deformVertex->boneWeight[2]
					+ matrix[3].d2 * deformVertex->boneWeight[3];

				outMatrix.d3 = matrix[0].d3 * deformVertex->boneWeight[0]
					+ matrix[1].d3 * deformVertex->boneWeight[1]
					+ matrix[2].d3 * deformVertex->boneWeight[2]
					+ matrix[3].d3 * deformVertex->boneWeight[3];

				outMatrix.d4 = matrix[0].d4 * deformVertex->boneWeight[0]
					+ matrix[1].d4 * deformVertex->boneWeight[1]
					+ matrix[2].d4 * deformVertex->boneWeight[2]
					+ matrix[3].d4 * deformVertex->boneWeight[3];
			}
			
			// apply animation matrix
			deformVertex->position = mesh->mVertices[v];
			deformVertex->position *= outMatrix;

			// for normal
			outMatrix.a4 = 0;
			outMatrix.b4 = 0;
			outMatrix.c4 = 0;

			deformVertex->normal = mesh->mNormals[v];
			deformVertex->normal *= outMatrix;

			// write to vertex buffer
			vertices[v].Position.x = deformVertex->position.x;
			vertices[v].Position.y = deformVertex->position.y;
			vertices[v].Position.z = deformVertex->position.z;

			vertices[v].Normal.x = deformVertex->normal.x;
			vertices[v].Normal.y = deformVertex->normal.y;
			vertices[v].Normal.z = deformVertex->normal.z;

			vertices[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertices[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertices[v].Diffuse = dx::XMFLOAT4(1, 1, 1, 1);
		}

		CRenderer::GetDeviceContext()->Unmap(m_vertexBuffer[m], 0);
	}
}

void Model::CreateBone(aiNode* node)
{
	Bone bone;
	m_bones[node->mName.C_Str()] = bone;

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		CreateBone(node->mChildren[n]);
	}
}

void Model::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	Bone* bone = &m_bones[node->mName.C_Str()];

	aiMatrix4x4 worldMatrix;
	worldMatrix = matrix;
	worldMatrix *= bone->animationMatrix;

	bone->matrix = worldMatrix;
	bone->matrix *= bone->offsetMatrix;

	for (unsigned int n = 0; n < node->mNumChildren; ++n)
	{
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}
