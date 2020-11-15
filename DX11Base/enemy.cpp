#include "pch.h"
#include "manager.h"
#include "modelmanager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "enemy.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "bullet.h"
#include "topdowncamera.h"


void Enemy::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_ENEMY, m_model);

	m_position = dx::XMFLOAT3(0.0F, 1.0F, 10.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 45, 0.0F);
	m_scale = dx::XMFLOAT3(1.0F, 1.0F, 1.0F);

	m_moveSpeed = 0.02F;

	obb.Init((GameObject*)this, 3, 5, 8);
}

void Enemy::Init()
{
	GameObject::Init();
	
	auto player = CManager::GetActiveScene()->GetGameObjects<Player>(0);
	if(!player.empty())
		m_rangeObject = player.front();
}

void Enemy::Uninit()
{
	GameObject::Uninit();
}

void Enemy::Update()
{
	GameObject::Update();
	obb.Update();

	m_rotation.y += 0.3F;
	m_rotation.z += 0.4F;
}

void Enemy::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	auto cam = CManager::GetActiveScene()->GetGameObjects<TopDownCamera>(0).front();
	if (!cam->InFrustum(m_position))
		return;

	// lighting
	LIGHT light;
	light.Enable = false;
	light.Direction = dx::XMFLOAT4(0.5F, -1.0F, 0.0F, 0.0F);
	dx::XMStoreFloat4(&light.Direction, dx::XMVector4Normalize(dx::XMLoadFloat4(&light.Direction)));

	light.Ambient = dx::XMFLOAT4(.1F, .1F, .1F, 1.0F);
	light.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetLight(light);

	// set the world matrix for this object based on lookat vector
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	// draw the model
	CRenderer::DrawModel(m_shader, m_model);
	obb.Draw();
}

void Enemy::Movement()
{
	if (auto player = m_rangeObject.lock())
	{
		dx::XMVECTOR direction = dx::XMVectorSubtract(player->GetPosition(), GetPosition());
		dx::XMStoreFloat3(&m_lookDirection, dx::XMVector3Normalize(direction));
		m_position += m_lookDirection * m_moveSpeed;
	}
}
