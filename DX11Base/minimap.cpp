#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "rendertexture.h"
#include "minimap.h"


void Minimap::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<MinimapShader>();
	m_renderTexture = new RenderTexture();

	CPolygon::CreatePlaneTopLeft(0, 0, 300, 300, m_VertexBuffer, false);
	CRenderer::BindRenderTargetView(m_renderTexture->GetRenderTargetView(), 2);
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

void Minimap::Draw(UINT renderPass)
{
	if (renderPass == 10)
	{
		GameObject::Draw(renderPass);

		// dont draw if vertex buffer is not initialized
		if (!m_VertexBuffer)
			return;

		// material
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_shader->SetMaterial(material);

		// texture
		m_shader->PS_SetTexture(m_Texture);
		m_shader->PS_SetTexture(m_renderTexture->GetRenderTexture());
		m_shader->PS_SetSamplerState(m_renderTexture->GetSamplerState());
		CRenderer::DrawPolygon(m_shader, &m_VertexBuffer, 4);
	}
}

void Minimap::SetTexture(const char* path)
{
	CPolygon::LoadTexture(path, m_Texture);
}
