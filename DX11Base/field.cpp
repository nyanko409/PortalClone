#include "pch.h"
#include "renderer.h"
#include "field.h"
#include "model.h"
#include "player.h"
#include "manager.h"
#include "bullet.h"


void Field::Init()
{
	GameObject::Init();

	// get the shader
	m_shader = CRenderer::GetShader<RangeShader>();

	// player position for shader
	m_rangeObject = CManager::GetActiveScene()->GetGameObjects<Player>(0).front();

	ModelManager::GetModel(MODEL_FLOOR, m_model);

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
	m_scale = dx::XMFLOAT3(200.0F, 1.0F, 200.0F);
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

void Field::Draw()
{
	GameObject::Draw();

	// set the active shader
	CRenderer::SetShader(m_shader);

	// lighting
	LIGHT light;
	light.Enable = true;
	light.Direction = dx::XMFLOAT4(0.5F, -1.0F, 0.0F, 0.0F);
	dx::XMStoreFloat4(&light.Direction, dx::XMVector4Normalize(dx::XMLoadFloat4(&light.Direction)));

	light.Ambient = dx::XMFLOAT4(.1F, .1F, .1F, 1.0F);
	light.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetLight(light);

	// set the world matrix for this object
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	// set buffers
	auto bullets = CManager::GetActiveScene()->GetGameObjects<Bullet>(0);
	if (auto player = m_rangeObject.lock())
	{
		if (!bullets.empty())
			m_shader->PS_SetRangeBuffer(10, player->GetPosition(), 5, bullets.back()->GetPosition());
		else
			m_shader->PS_SetRangeBuffer(10, player->GetPosition(), -1, dx::XMVECTOR{ 0,0,0 });
	}

	m_shader->PS_SetNormalTexture(m_normalTexture);
	m_shader->PS_SetValueBuffer(8, true, false);

	m_model->Draw(m_shader);
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
