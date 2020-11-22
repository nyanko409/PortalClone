#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "field.h"
#include "modelmanager.h"
#include "manager.h"
#include "bullet.h"
#include "light.h"


void Field::Init()
{
	GameObject::Init();

	// get the shader
	m_shader = CRenderer::GetShader<RangeShader>();
	m_depthShader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_FLOOR, m_model);

	// player position for shader
	m_rangeObject = CManager::GetActiveScene()->GetGameObjects<Player>(0).front();

	// normal texture
	D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
		"asset/texture/hexagon_normal.tif",
		NULL,
		NULL,
		&m_normalTexture,
		NULL);

	assert(m_normalTexture);

	// init values
	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(20, 1.0F, 20);
}

void Field::Uninit()
{
	GameObject::Uninit();

	if (m_normalTexture)
	{
		m_normalTexture->Release();
		m_normalTexture = nullptr;
	}
}

void Field::Update()
{
	GameObject::Update();
}

void Field::Draw(UINT renderPass)
{
	if (renderPass == 2)
	{
		GameObject::Draw(renderPass);

		// set light buffer
		m_shader->SetDirectionalLight(LightManager::GetDirectionalLight());

		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		auto bullets = CManager::GetActiveScene()->GetGameObjects<Bullet>(0);
		if (auto player = m_rangeObject.lock())
		{
			dx::XMVECTOR pos = player->GetPosition();

			if (!bullets.empty())
				m_shader->SetRangeBuffer(player->GetSightRange(), pos, 5, bullets.back()->GetPosition());
			else
				m_shader->SetRangeBuffer(player->GetSightRange(), pos, -1, dx::XMVECTOR{ 0,0,0 });
		}
		
		m_shader->SetNormalTexture(m_normalTexture);
		m_shader->SetValueBuffer(8, true, false);

		// draw
		CRenderer::DrawModel(m_shader, m_model);
	}
	else if (renderPass == 1)
	{
		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_depthShader->SetWorldMatrix(&world);

		// draw the model
		CRenderer::DrawModel(m_depthShader, m_model);
	}
}

int Field::CheckBounds(dx::XMFLOAT3 position, float objectRadius)
{
	int result = 0;

	if (position.x + objectRadius > m_scale.x || position.x - objectRadius < -m_scale.x)
		result |= 0b0001;
	if (position.z + objectRadius > m_scale.z || position.z - objectRadius < -m_scale.z)
		result |= 0b0010;

	return result;
}
