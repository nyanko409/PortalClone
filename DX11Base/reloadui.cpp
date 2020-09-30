#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "reloadui.h"


void ReloadUI::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<UIShader>();
	m_fillSpeed = 3;

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

void ReloadUI::Draw(UINT renderPass)
{
	if (renderPass == 1)
	{
		GameObject::Draw(renderPass);

		// remap vertex buffer to adjust width
		CPolygon::RemapDimensionsTopLeft(m_posX, m_posY, m_gaugeWidth, m_height, m_vertexBufferGauge);

		// disable texture
		m_shader->PS_SetValueBuffer(false);

		// background material
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = dx::XMFLOAT4(0.5F, 0.5F, 0.5F, 1.0F);
		m_shader->SetMaterial(material);

		// draw background
		CRenderer::DrawPolygon(m_shader, &m_vertexBuffer, 4);

		// gauge material
		material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_shader->SetMaterial(material);

		// draw gauge
		CRenderer::DrawPolygon(m_shader, &m_vertexBufferGauge, 4);
	}
}

void ReloadUI::FillGauge()
{
	m_gaugeWidth += m_fillSpeed;
	if (m_gaugeWidth > m_width)
		m_gaugeWidth = m_width;
}
