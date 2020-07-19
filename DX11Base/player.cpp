#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "math.h"
#include "input.h"


void Player::Init()
{
	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(0.1F, 0.1F, 0.1F);
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
	dx::XMMATRIX scale, rot, trans;
	scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	rot = dx::XMMatrixRotationRollPitchYaw(m_rotation.y, m_rotation.x, m_rotation.z);
	trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	CRenderer::SetWorldMatrix(&(scale * rot * trans));

	m_model->Draw();
}
