#include "pch.h"
#include "main.h"
#include "renderer.h"
#include "fade.h"


void Fade::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<UIShader>();
	m_alpha = 0;
	m_isFadingIn = m_isFadingOut = false;

	//CPolygon::LoadTexture("asset/texture/noise.png", m_Texture);
	CPolygon::CreatePlaneCenter(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT, m_VertexBuffer, false);
}

void Fade::Uninit()
{
	GameObject::Uninit();

	SAFE_RELEASE(m_Texture);
	SAFE_RELEASE(m_VertexBuffer);
}

void Fade::Update()
{
	GameObject::Update();

	// fade if flag is set
	if (m_isFadingOut)
	{
		m_alpha -= m_fadeSpeed;
		if (m_alpha < 0.0F)
		{
			m_alpha = 0.0F;
			m_isFadingOut = false;

			if(m_function)
				m_function();
		}
	}
	else if (m_isFadingIn)
	{
		m_alpha += m_fadeSpeed;
		if (m_alpha > 1.0F)
		{
			m_alpha = 1.0F;
			m_isFadingIn = false;

			if(m_function)
				m_function();
		}
	}
}

void Fade::Draw()
{
	GameObject::Draw();

	// set shader
	CRenderer::SetShader(m_shader);

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// material
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(0.0F, 0.0F, 0.0F, m_alpha);
	m_shader->SetMaterial(material);

	//プリミティブトポロジー設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ポリゴン描画
	CRenderer::GetDeviceContext()->Draw(4, 0);
}

void Fade::StartFadeOut(float fadeSpeed, onFinished function)
{
	m_alpha = 1;
	m_isFadingOut = true;
	m_fadeSpeed = fadeSpeed;
	m_function = function;
}

void Fade::StartFadeIn(float fadeSpeed, onFinished function)
{
	m_alpha = 0;
	m_isFadingIn = true;
	m_fadeSpeed = fadeSpeed;
	m_function = function;
}
