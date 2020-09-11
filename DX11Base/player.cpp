#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "bullet.h"
#include "field.h"
#include "reloadui.h"
#include "enemy.h"
#include "billboard.h"
#include "fade.h"
#include "scenetitle.h"


void Player::Init()
{
	GameObject::Init();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(0.05F, 0.05F, 0.05F);

	m_moveSpeed = 0.1F;
	m_sightRange = 15;
}

void Player::Uninit()
{
	GameObject::Uninit();
}

void Player::Update()
{
	GameObject::Update();

	static float frame = 0;
	frame += .5F;

		m_model->Update(frame, 0);
	if (CInput::GetKeyPress(DIK_2))
		m_model->Update(frame, 2);

	Movement();
	IdleAnimation();
	GetLookAtDirection();

	// shoot with mouse click
	if (CInput::GetMouseLeftTrigger())
	{
		auto gauge = CManager::GetActiveScene()->GetGameObjects<ReloadUI>(2).front();
		if (gauge->IsGaugeFilled())
		{
			ShootProjectile();
			gauge->ResetGauge();
		}
	}

	// decrease sight range over time
	m_sightRange -= 0.0015F;
	if (m_sightRange <= 5)
		m_sightRange = 5;

	// basic collision with bounds of field
	int col = CManager::GetActiveScene()->GetGameObjects<Field>(0).front()->CheckBounds(m_position, 1);
	if (col & 0b0001) m_position.x = m_oldPosition.x;
	if (col & 0b0010) m_position.z = m_oldPosition.z;

	// basic collision with enemies
	auto enemies = CManager::GetActiveScene()->GetGameObjects<Enemy>(1);
	for (auto enemy : enemies)
	{
		float distance = dx::XMVectorGetX(dx::XMVector3Length(dx::XMVectorSubtract(enemy->GetPosition(), GetPosition())));
		if (distance < 2)
		{
			// play explosion effect
			auto effect = CManager::GetActiveScene()->AddGameObject<Billboard>(1);
			effect->SetPosition(m_position + dx::XMFLOAT3{0, 1, 0});
			effect->SetScale(3, 3, 3);
			Audio::PlaySoundA(AUDIO_SE_EXPLOSION);
	
			// fade in to title scene
			auto fade = CManager::GetActiveScene()->AddGameObject<Fade>(2);
			fade->StartFadeIn(0.005F, CManager::SetScene<Title>);
	
			SetDestroy();
			return;
		}
	}
}

void Player::Draw()
{
	GameObject::Draw();

	// lighting
	LIGHT light;
	light.Enable = true;
	light.Direction = dx::XMFLOAT4(0.5F, -1.0F, 0.0F, 0.0F);
	dx::XMStoreFloat4(&light.Direction, dx::XMVector4Normalize(dx::XMLoadFloat4(&light.Direction)));

	light.Ambient = dx::XMFLOAT4(.1F, .1F, .1F, 1.0F);
	light.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetLight(light);

	// set the world matrix for this object based on mouse lookat vector
	dx::XMMATRIX world = GetWorldMatrix();

	dx::XMFLOAT4X4 t;
	dx::XMStoreFloat4x4(&t, world);

	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 1);
	dx::XMVECTOR zaxis = dx::XMVector3Normalize(dx::XMVectorSet(m_lookAtDirection.x, 0, m_lookAtDirection.z, 1));
	dx::XMVECTOR xaxis = dx::XMVector3Normalize(dx::XMVector3Cross(up, zaxis));
	dx::XMVECTOR yaxis = dx::XMVector3Cross(zaxis, xaxis);

	dx::XMFLOAT3 z, x, y;
	dx::XMStoreFloat3(&z, zaxis);
	dx::XMStoreFloat3(&x, xaxis);
	dx::XMStoreFloat3(&y, yaxis);

	t._11 = x.x * m_scale.x; t._12 = x.y * m_scale.y; t._13 = x.z * m_scale.z;
	t._21 = y.x * m_scale.x; t._22 = y.y * m_scale.y; t._23 = y.z * m_scale.z;
	t._31 = z.x * m_scale.x; t._32 = z.y * m_scale.y; t._33 = z.z * m_scale.z;

	world = dx::XMLoadFloat4x4(&t);
	m_shader->SetWorldMatrix(&world);

	// draw the model
	CRenderer::DrawModel(m_shader, m_model);
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

void Player::GetLookAtDirection()
{
	// get the mouse coordinate and normalize it from -1 to 1
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(GetWindow(), &point);

	// offset it for camera angle
	point.x += 0;
	point.y += 30;

	float x, z;
	x = (float)(point.x - SCREEN_WIDTH / 2.0F) / SCREEN_WIDTH * 2;
	z = (float)(point.y - SCREEN_HEIGHT / 2.0F) / -SCREEN_HEIGHT * 2;

	dx::XMVECTOR norm = dx::XMVectorSet(x, 0, z,1);
	norm = dx::XMVector3Normalize(norm);
	dx::XMStoreFloat3(&m_lookAtDirection, norm);
}

void Player::IdleAnimation()
{
	m_position.y = (fabsf(sinf(dx::XMConvertToRadians(m_idleYPos))) * 0.2F) + 0.1F;
	m_idleYPos += 1.F;
}

void Player::ShootProjectile()
{
	auto bullet = CManager::GetActiveScene()->AddGameObject<Bullet>(0);
	bullet->SetPosition(m_position);
	bullet->SetDirection(m_lookAtDirection);
}
