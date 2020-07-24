#include "pch.h"
#include "renderer.h"
#include "field.h"


void Field::Init()
{
	GameObject::Init();

	VERTEX_3D vertex[4];
	float width = 20;
	float height = 0;
	float depth = 20;
	float halfWidth = width / 2;
	float halfHeight = height / 2;
	float halfDepth = depth / 2;

	vertex[0].Position = dx::XMFLOAT3(-halfWidth, halfHeight, halfDepth);
	vertex[0].Normal = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[0].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = dx::XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = dx::XMFLOAT3(halfWidth, halfHeight, halfDepth);
	vertex[1].Normal = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[1].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = dx::XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = dx::XMFLOAT3(-halfWidth, -halfHeight, -halfDepth);
	vertex[2].Normal = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[2].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = dx::XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = dx::XMFLOAT3(halfWidth, -halfHeight, -halfDepth);
	vertex[3].Normal = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertex[3].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].TexCoord = dx::XMFLOAT2(1.0f, 1.0f);

	//頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertex;

	CRenderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);

	//テクスチャ読み込み
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/stonks.jpg",
		NULL,
		NULL,
		&m_Texture,
		NULL);

	assert(m_Texture);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(1.0F, 1.0F, 1.0F);

	m_quaternion = dx::XMFLOAT4(0, 0, 0, 1);
	m_prevRotation = m_rotation;
	m_diffRotation = dx::XMFLOAT3(0, 0, 0);
}

void Field::Uninit()
{
	GameObject::Uninit();

	m_VertexBuffer->Release();
	m_Texture->Release();
}

void Field::Update()
{
	GameObject::Update();
}

void Field::Draw()
{
	GameObject::Draw();

	// set the world matrix for this object
	dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_quaternion);

	dx::XMMATRIX scale, rot, trans;
	scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	rot = dx::XMMatrixRotationQuaternion(quaternion);
	trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	CRenderer::SetWorldMatrix(&(scale * rot * trans));

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// set material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	CRenderer::SetMaterial(material);

	//テクスチャ設定
	CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//プリミティブトポロジー設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	CRenderer::GetDeviceContext()->Draw(4, 0);
}
