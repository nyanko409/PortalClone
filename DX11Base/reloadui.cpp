#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "reloadui.h"


void ReloadUI::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<UIShader>();
	m_fillSpeed = 2.F;

	m_posX = (SCREEN_WIDTH / 2) - 150; m_posY = (SCREEN_HEIGHT / 2) + 200;
	m_width = 300; m_height = 20;
	m_gaugeWidth = 0;
	CPolygon::CreatePlaneTopLeft(m_posX, m_posY, m_width, m_height, m_vertexBuffer, false);
	CPolygon::CreatePlaneTopLeft(m_posX, m_posY, m_width, m_height, m_vertexBufferGauge, true);
}

void ReloadUI::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_vertexBufferGauge);
}

void ReloadUI::Update()
{
	GameObject::Update();

	FillGauge();
}

void ReloadUI::Draw()
{
	GameObject::Draw();

	// remap vertex buffer to adjust width
	CPolygon::RemapDimensionsTopLeft(m_posX, m_posY, m_gaugeWidth, m_height, m_vertexBufferGauge);

	// set shader
	CRenderer::SetShader(m_shader);
	m_shader->PS_SetValueBuffer(false);

	//プリミティブトポロジー設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(0.0F, 0.0F, 0.0F, 1.0F);
	m_shader->SetMaterial(material);

	// draw background
	CRenderer::GetDeviceContext()->Draw(4, 0);

	// draw gauge
	material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetMaterial(material);
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBufferGauge, &stride, &offset);
	CRenderer::GetDeviceContext()->Draw(4, 0);
}

void ReloadUI::FillGauge()
{
	m_gaugeWidth += m_fillSpeed;
	if (m_gaugeWidth > m_width)
		m_gaugeWidth = m_width;
}
