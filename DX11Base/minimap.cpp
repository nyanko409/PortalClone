#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "minimap.h"
#include "polygon.h"


void Minimap::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<MinimapShader>();
	//m_renderTexture = std::make_shared<RenderTexture>(2, SCREEN_WIDTH, SCREEN_HEIGHT, false);

	CPolygon::CreatePlaneTopLeft(0, 0, 300, 300, m_VertexBuffer, false);
	CRenderer::BindRenderTargetView(m_renderTexture);
}

void Minimap::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_Texture);
	SAFE_RELEASE(m_VertexBuffer);
}

void Minimap::Update()
{
	GameObject::Update();
}

void Minimap::Draw(Pass pass)
{
	if (pass == Pass::Default)
	{
		GameObject::Draw(pass);

		// dont draw if vertex buffer is not initialized
		if (!m_VertexBuffer)
			return;

		// material
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_shader->SetMaterial(material);

		// bind the render texture
		m_shader->SetTexture(m_renderTexture->GetRenderTexture());

		// draw
		CRenderer::DrawPolygon(m_shader, &m_VertexBuffer, 4);

		// unbind render texture
		m_shader->SetTexture(nullptr);
	}
}

void Minimap::SetTexture(const char* path)
{
	CPolygon::LoadTexture(path, m_Texture);
}
