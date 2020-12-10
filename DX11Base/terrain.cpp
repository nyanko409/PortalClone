#include "pch.h"
#include "terrain.h"
#include "player.h"
#include "manager.h"
#include "light.h"
#include "main.h"
#include "Camera.h"
#include "rendertexture.h"


void Terrain::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<BasicLightShader>();
	m_enableFrustumCulling = false;

	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/Ground.tif",
		NULL,
		NULL,
		&m_texture,
		NULL);

	assert(m_texture);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(1.0F, 1.0F, 1.0F);
}

void Terrain::CreateTerrain(int size)
{
	size++;

	// release previous allocated memory
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);

	if (m_size > 0)
	{
		for (int i = 0; i <= m_size; ++i)
			free(m_vertices[i]);
	}

	// create vertex buffer
	m_size = size - 1;

	VERTEX_3D* vertex = (VERTEX_3D*)malloc(sizeof(VERTEX_3D) * size * size);
	m_vertices = (VERTEX_3D**)malloc(size * sizeof(VERTEX_3D*));
	for (int i = 0; i < size; i++)
		m_vertices[i] = (VERTEX_3D*)malloc(size * sizeof(VERTEX_3D));

	int count = 0;
	float offset = (size - 1) / 2.0F;
	for (int x = 0; x < size; ++x)
	{
		for (int z = 0; z < size; ++z)
		{
			float h = sinf(x * 0.1F) * 2.0F * sinf(z * 0.1F) * 2.0F;
			vertex[count].Position = dx::XMFLOAT3(x - offset, h, -z + offset);
			vertex[count].Normal = dx::XMFLOAT3(0.0F, 1.0F, 0.0F);
			vertex[count].Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
			vertex[count].TexCoord = dx::XMFLOAT2(x / (size - 1.0F), z / (size - 1.0F));

			m_vertices[z][x] = vertex[count];

			count++;
		}
	}

	// normal
	//for (int x = 0; x < size; ++x)
	//{
	//	for (int z = 0; z < size; ++z)
	//	{
	//		dx::XMFLOAT3 p12, p13;
	//
	//		p12 = m_vertices[x + 1][z].Position - m_vertices[x][z].Position;
	//	}
	//}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(VERTEX_3D) * size * size;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
	free(vertex);

	// create index buffer
	int indexNum = 4;
	if (size > 1)
	{
		int nextSize = 3;
		for (int i = 0; i < size - 2; ++i)
		{
			indexNum += 10 + 4 * (nextSize - 3);
			nextSize++;
		}
	}

	m_indexCount = indexNum;

	unsigned int* index = (unsigned int*)malloc(sizeof(unsigned int) * m_indexCount);
	int counter = size;
	int dummyCounter = 0;
	for (int i = 0; i < m_indexCount; ++i)
	{
		if (dummyCounter == size * 2)
		{
			index[i] = counter - 1;
		}
		else if (dummyCounter == size * 2 + 1)
		{
			index[i] = counter - 1 - size + 1;
			dummyCounter = 0;
			continue;
		}
		else
		{
			if (i % 2 == 0)
			{
				index[i] = counter - size;
			}
			else
			{
				index[i] = counter;
				counter++;
			}
		}

		dummyCounter++;
	}

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * m_indexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = index;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_indexBuffer);
	free(index);
}

void Terrain::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_texture);

	if (m_size > 0)
	{
		for (int i = 0; i <= m_size; ++i)
			free(m_vertices[i]);
	}
}

void Terrain::Update()
{
	GameObject::Update();
}

void Terrain::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	if (renderPass == 1)
	{
		// set shader buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);
		m_shader->SetTexture(m_texture);

		m_shader->SetDirectionalLight(LightManager::GetDirectionalLight());

		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_shader->SetMaterial(material);

		m_shader->SetShadowMapTexture(CRenderer::GetRenderTexture(2)->GetRenderTexture());
		m_shader->SetLightProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
		m_shader->SetLightViewMatrix(&LightManager::GetDirectionalViewMatrix());

		// draw
		CRenderer::DrawPolygonIndexed(m_shader, &m_vertexBuffer, m_indexBuffer, m_indexCount);
	}
}

void Terrain::Draw(const std::shared_ptr<Shader>& shader, UINT renderPass)
{
	GameObject::Draw(shader, renderPass);

	// set shader buffers
	dx::XMMATRIX world = GetWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetDirectionalViewMatrix());

	// draw the model
	CRenderer::DrawPolygonIndexed(shader, &m_vertexBuffer, m_indexBuffer, m_indexCount);
}

float Terrain::GetHeight(dx::XMFLOAT3 position)
{
	int x, z;
	float halfSize = m_size / 2.0F;
	
	x = position.x + halfSize;
	z = -position.z + halfSize;
	
	dx::XMFLOAT3 pos0, pos1, pos2, pos3;
	
	pos0 = m_vertices[z][x].Position;
	pos1 = m_vertices[z + 1][x].Position;
	pos2 = m_vertices[z][x + 1].Position;
	pos3 = m_vertices[z + 1][x + 1].Position;
	
	dx::XMVECTOR v12, v1p, c;
	
	v12 = dx::XMLoadFloat3(&(pos2 - pos1));
	v1p = dx::XMLoadFloat3(&(position - pos1));
	
	c = dx::XMVector3Cross(v12, v1p);
	
	dx::XMVECTOR n;
	if (dx::XMVectorGetY(c) > 0.0F)
	{
		// left-up polygon
		dx::XMVECTOR v10;
		v10 = dx::XMLoadFloat3(&(pos0 - pos1));
		n = dx::XMVector3Cross(v10, v12);
	}
	else
	{
		// right-bottom polygon
		dx::XMVECTOR v13;
		v13 = dx::XMLoadFloat3(&(pos3 - pos1));
		n = dx::XMVector3Cross(v12, v13);
	}
	
	return -((position.x - pos1.x) * dx::XMVectorGetX(n) + (position.z - pos1.z) * dx::XMVectorGetZ(n)) / dx::XMVectorGetY(n) + pos1.y;
}
