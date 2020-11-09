#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "math.h"
#include "input.h"
#include "billboard.h"
#include "modelmanager.h"
#include "bullet.h"
#include "field.h"
#include "enemy.h"


void Bullet::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<BasicLightShader>();
	ModelManager::GetModel(MODEL_BULLET, m_model);

	m_speed = 0.4F;
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

	// basic collision with enemies
	auto enemies = CManager::GetActiveScene()->GetGameObjects<Enemy>(1);
	for (auto enemy : enemies)
	{
		float distance = dx::XMVectorGetX(dx::XMVector3Length(dx::XMVectorSubtract(enemy->GetPosition(), GetPosition())));
		if (distance < 1.2F)
		{
			auto effect = CManager::GetActiveScene()->AddGameObject<Billboard>(1);
			effect->SetPosition(dx::XMVectorAdd(enemy->GetPosition(), dx::XMVECTOR{0,1,0}));
			effect->SetScale(4, 4, 1);
			Audio::PlaySoundA(AUDIO_SE_EXPLOSION);

			enemy->SetDestroy();
			SetDestroy();

			return;
		}
	}
}

void Bullet::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	// set the world matrix for this object
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	// draw the model using the world matrix
	CRenderer::DrawModel(m_shader, m_model);
}
