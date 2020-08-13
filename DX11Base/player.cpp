#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "math.h"
#include "input.h"


void Player::Init()
{
	GameObject::Init();

	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(.5F, .5F, .5F);

	m_quaternion = dx::XMFLOAT4(0, 0, 0, 1);
	m_prevRotation = m_rotation;
	m_diffRotation = dx::XMFLOAT3(0, 0, 0);
}

void Player::Uninit()
{
	GameObject::Uninit();
}

void Player::Update()
{
	GameObject::Update();

	m_rotation.y += 0.01F;
	if (CInput::GetKeyPress('G'))
	{
		m_position -= GetRight() * 0.1F;
	}
	if (CInput::GetKeyPress('J'))
	{
		m_position += GetRight() * 0.1F;
	}
	if (CInput::GetKeyPress('Y'))
	{
		m_position += GetForward() * 0.1F;
	}
	if (CInput::GetKeyPress('H'))
	{
		m_position -= GetForward() * 0.1F;
	}
}

void Player::Draw()
{
	GameObject::Draw();

	// set the active shader
	CRenderer::SetShader(m_shader);

	// set the world matrix for this object
	dx::XMVECTOR quaternion = dx::XMLoadFloat4(&m_quaternion);

	dx::XMMATRIX scale, rot, trans;
	scale = dx::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	rot = dx::XMMatrixRotationQuaternion(quaternion);
	trans = dx::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_shader->SetWorldMatrix(&(scale * rot * trans));

	// draw the model using the world matrix
	m_model->Draw(m_shader);
}
