#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "polygon.h"


void CPolygon::Init()
{
	GameObject::Init();

	VERTEX_3D vertex[4];
	float posX = SCREEN_WIDTH / 2.0F;
	float posY = SCREEN_HEIGHT / 2.0F;
	float width = 32;
	float height = 32;
	float halfWidth = width / 2;
	float halfHeight = height / 2;

	vertex[0].Position = dx::XMFLOAT3(posX - halfWidth, posY - halfHeight, 0.0f);
	vertex[0].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[0].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[0].TexCoord = dx::XMFLOAT2(0.0f, 0.0f);

	vertex[1].Position = dx::XMFLOAT3(width + posX - halfWidth, posY - halfHeight, 0.0f);
	vertex[1].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[1].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].TexCoord = dx::XMFLOAT2(1.0f, 0.0f);

	vertex[2].Position = dx::XMFLOAT3(posX - halfWidth, height + posY - halfHeight, 0.0f);
	vertex[2].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	vertex[2].Diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].TexCoord = dx::XMFLOAT2(0.0f, 1.0f);

	vertex[3].Position = dx::XMFLOAT3(width + posX - halfWidth, height + posY - halfHeight, 0.0f);
	vertex[3].Normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
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
		"asset/texture/crosshair.png",
		NULL,
		NULL,
		&m_Texture,
		NULL);
}

void CPolygon::Uninit()
{
	GameObject::Uninit();

	m_VertexBuffer->Release();
	m_Texture->Release();
}

void CPolygon::Update()
{
	GameObject::Update();
}

void CPolygon::Draw()
{
	//GameObject::Draw();
	//
	////頂点バッファ設定
	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	//
	//MATERIAL material;
	//ZeroMemory(&material, sizeof(material));
	//material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	//CRenderer::SetMaterial(material);
	//
	////テクスチャ設定
	//CRenderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	//
	////プリミティブトポロジー設定
	//CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//
	////ポリゴン描画
	//CRenderer::GetDeviceContext()->Draw(4, 0);
}
