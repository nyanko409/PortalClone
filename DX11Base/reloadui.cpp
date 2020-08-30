#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "reloadui.h"


void ReloadUI::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<UIShader>();

	CPolygon::CreatePlane(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, 50, m_vertexBuffer, true);
}

void ReloadUI::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_vertexBuffer);
}

void ReloadUI::Update()
{
	GameObject::Update();
}

void ReloadUI::Draw()
{
	GameObject::Draw();

	// set shader
	CRenderer::SetShader(m_shader);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(0.0F, 0.0F, 0.0F, 1.0F);
	m_shader->SetMaterial(material);

	//プリミティブトポロジー設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	CRenderer::GetDeviceContext()->Draw(4, 0);
}
