#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "sprite.h"


void Sprite::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<UIShader>();
}

void Sprite::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_Texture);
	SAFE_RELEASE(m_VertexBuffer);
}

void Sprite::Update()
{
	GameObject::Update();
}

void Sprite::Draw()
{
	GameObject::Draw();

	// dont draw if vertex buffer is not initialized
	if (!m_VertexBuffer)
		return;

	// set shader
	CRenderer::SetShader(m_shader);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetMaterial(material);

	// texture
	if (m_Texture)
	{
		m_shader->PS_SetValueBuffer(true);
		m_shader->PS_SetTexture(m_Texture);
	}
	else
		m_shader->PS_SetValueBuffer(false);

	//プリミティブトポロジー設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	CRenderer::GetDeviceContext()->Draw(4, 0);
}

void Sprite::SetTexture(const char* path)
{
	CPolygon::LoadTexture(path, m_Texture);
}

void Sprite::CreatePlaneCenter(float centerX, float centerY, float width, float height, bool dynamic)
{
	SAFE_RELEASE(m_VertexBuffer);
	CPolygon::CreatePlaneCenter(centerX, centerY, width, height, m_VertexBuffer, dynamic);
}

void Sprite::CreatePlaneTopLeft(float topLeftX, float topLeftY, float width, float height, bool dynamic)
{
	SAFE_RELEASE(m_VertexBuffer);
	CPolygon::CreatePlaneTopLeft(topLeftX, topLeftY, width, height, m_VertexBuffer, dynamic);
}

void Sprite::SetPositionCenter(float centerX, float centerY, float width, float height)
{
	CPolygon::RemapDimensionsCenter(centerX, centerY, width, height, m_VertexBuffer);
}

void Sprite::SetPositionTopLeft(float topLeftX, float topLeftY, float width, float height)
{
	CPolygon::RemapDimensionsTopLeft(topLeftX, topLeftY, width, height, m_VertexBuffer);
}
