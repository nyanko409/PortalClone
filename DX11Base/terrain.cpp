#include "pch.h"
#include "terrain.h"


void Terrain::Init()
{
	GameObject::Init();

	m_shader = CRenderer::GetShader<BasicLightShader>();
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/noise.png",
		NULL,
		NULL,
		&m_texture,
		NULL);

	assert(m_texture);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(1.0F, 1.0F, 1.0F);

	// create vertex buffer
	const int size = 9 + 1;

	VERTEX_3D vertex[size * size];
	int count = 0;
	float offset = (size - 1) / 2.0F;
	for (int x = 1; x <= size; ++x)
	{
		for (int y = 0; y < size; ++y)
		{
			vertex[count].Position = dx::XMFLOAT3(x - 1 - offset, 0.0F, -y + offset);
			vertex[count].Normal = dx::XMFLOAT3(0.0F, 1.0F, 0.0F);
			vertex[count].Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
			vertex[count].TexCoord = dx::XMFLOAT2((x - 1.0F) / (size - 1.0F), y / (size - 1.0F));

			count++;
		}
	}

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
			index[i] = counter - 1 - size - 1;
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
}

void Terrain::Update()
{
	GameObject::Update();
}

void Terrain::Draw()
{
	GameObject::Draw();

	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);
	m_shader->PS_SetTexture(m_texture);

	// set material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetMaterial(material);

	// draw the model
	CRenderer::DrawPolygonIndexed(m_shader, &m_vertexBuffer, m_indexBuffer, m_indexCount);
}
