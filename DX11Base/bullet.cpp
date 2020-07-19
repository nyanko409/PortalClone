#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "math.h"
#include "input.h"
#include "billboard.h"
#include "bullet.h"


void Bullet::Init()
{
	ModelManager::GetModel(MODEL_BULLET, m_model);

	m_speed = 0.5F;
	m_timeToLive = 3.0F;
	m_timeTillStart = 0.0F;

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(0.1F, 0.1F, 0.1F);
}

void Bullet::Uninit()
{

}

void Bullet::Update()
{
	// destroy if this object lived its life :)
	m_timeTillStart += TIME_PER_FRAME;
	if (m_timeTillStart >= m_timeToLive)
	{
		SetDestroy();
		return;
	}

	// move this object towards direction
	m_position += m_direction * m_speed;

	// easy collision with player
	auto playerList = CManager::GetActiveScene()->GetGameObjects<Player>(0);
	for (Player* player : playerList)
	{
		dx::XMVECTOR direction = dx::XMVectorSubtract(GetPosition(), player->GetPosition());
		direction = dx::XMVector3Length(direction);

		float length;
		dx::XMStoreFloat(&length, direction);

		if (length < 2.0F)
		{
			player->SetDestroy();
			SetDestroy();
			return;
		}
	}
}

void Bullet::Draw()
{
	//マトリックス設定
	dx::XMMATRIX scale, rot, trans;
	scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	rot = dx::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z);
	trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	CRenderer::SetWorldMatrix(&(scale * rot * trans));

	m_model->Draw();
}
