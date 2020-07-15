#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "math.h"
#include "input.h"


void Player::Init()
{
	ModelManager::GetModel(ModelType::MODEL_PLAYER, m_model);

	m_position = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
	m_rotation = D3DXVECTOR3(0.0F, 0.0F, 0.0F);
	m_scale = D3DXVECTOR3(0.1F, 0.1F, 0.1F);
}

void Player::Uninit()
{
	
}

void Player::Update()
{
	
}

void Player::Draw()
{
	//マトリックス設定
	D3DXMATRIX scale, rot, trans;
	D3DXMatrixScaling(&scale, m_scale.x, m_scale.y, m_scale.z);
	D3DXMatrixRotationYawPitchRoll(&rot, m_rotation.y, m_rotation.x, m_rotation.z);
	D3DXMatrixTranslation(&trans, m_position.x, m_position.y, m_position.z);

	CRenderer::SetWorldMatrix(&(scale * rot * trans));

	m_model->Draw();
}
