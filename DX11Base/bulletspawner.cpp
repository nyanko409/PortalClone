#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "bulletspawner.h"
#include "math.h"
#include "input.h"
#include "player.h"
#include "bullet.h"


void BulletSpawner::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(1, 1, 1);

	//SetParent(CManager::GetActiveScene()->GetGameObjects<Player>(0).front());
}

void BulletSpawner::Uninit()
{
	GameObject::Uninit();
}

void BulletSpawner::Update()
{
	GameObject::Update();

	if (CInput::GetKeyTrigger(DIK_SPACE))
	{
		auto bullet = CManager::GetActiveScene()->AddGameObject<Bullet>(0);
		bullet->SetDirection(GetForward());
		bullet->SetPosition(GetPosition());
	}
}

void BulletSpawner::Draw()
{
	GameObject::Draw();

	// set the active shader
	CRenderer::SetShader(m_shader);

	// set the world matrix for this object
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	// draw the model
	m_model->Draw(m_shader);
}
