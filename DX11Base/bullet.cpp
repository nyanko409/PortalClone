#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "math.h"
#include "input.h"
#include "billboard.h"
#include "bullet.h"
#include "player.h"
#include "field.h"


void Bullet::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<BasicLightShader>();
	ModelManager::GetModel(MODEL_BULLET, m_model);

	m_speed = 0.5F;
	m_timeToLive = 3.0F;
	m_timeTillStart = 0.0F;

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(0.3F, 0.3F, 0.3F);
}

void Bullet::Uninit()
{
	GameObject::Uninit();
}

void Bullet::Update()
{
	GameObject::Update();

	// destroy if this object lived its life :)
	m_timeTillStart += TIME_PER_FRAME;
	if (m_timeTillStart >= m_timeToLive)
	{
		SetDestroy();
		return;
	}

	// move this object towards direction
	m_position += m_direction * m_speed;

	// basic collision with bounds of field
	if (CManager::GetActiveScene()->GetGameObjects<Field>(0).front()->CheckBounds(m_position, 0))
	{
		SetDestroy();
	}
}

void Bullet::Draw()
{
	GameObject::Draw();

	// set the active shader
	CRenderer::SetShader(m_shader);

	// set the world matrix for this object
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	// draw the model using the world matrix
	m_model->Draw(m_shader);
}
