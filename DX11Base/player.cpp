#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "math.h"
#include "input.h"
#include "topdowncamera.h"


void Player::Init()
{
	GameObject::Init();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(.2F, .2F, .2F);

	m_moveSpeed = 0.1F;

	// set the target of the top down camera
	CManager::GetActiveScene()->GetGameObjects<TopDownCamera>(0).front()->SetTarget(this);
}

void Player::Uninit()
{
	GameObject::Uninit();
}

void Player::Update()
{
	GameObject::Update();

	Movement();
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

	// draw the model
	m_model->Draw(m_shader);
}

void Player::Movement()
{
	// normalized wasd movement
	dx::XMVECTOR moveDirection = dx::XMVectorZero();
	if (CInput::GetKeyPress(DIK_W))
		moveDirection += {0, 0, 1};
	if (CInput::GetKeyPress(DIK_A))
		moveDirection -= {1, 0, 0};
	if (CInput::GetKeyPress(DIK_S))
		moveDirection -= {0, 0, 1};
	if (CInput::GetKeyPress(DIK_D))
		moveDirection += {1, 0, 0};

	moveDirection = dx::XMVector3Normalize(moveDirection);
	m_position += dx::XMVectorScale(moveDirection, m_moveSpeed);
}
