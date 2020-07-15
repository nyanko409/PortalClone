#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "math.h"
#include "input.h"
#include "bullet.h"


void Bullet::Init()
{
	ModelManager::GetModel(MODEL_BULLET, m_model);

	m_speed = 0.5F;
	m_timeToLive = 3.0F;
	m_timeTillStart = 0.0F;

	m_position = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
	m_rotation = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
	m_scale = D3DXVECTOR3(0.1F, 0.1F, 0.1F);
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
		D3DXVECTOR3 playerPos = player->GetPosition();
		D3DXVECTOR3 direction = m_position - playerPos;

		if (D3DXVec3Length(&direction) < 2.0F)
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
	D3DXMATRIX scale, rot, trans;
	D3DXMatrixScaling(&scale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixRotationYawPitchRoll(&rot, m_rotation.y, m_rotation.x, m_rotation.z);
	D3DXMatrixTranslation(&trans, m_position.x, m_position.y, m_position.z);

	CRenderer::SetWorldMatrix(&(scale * rot * trans));

	m_model->Draw();
}
