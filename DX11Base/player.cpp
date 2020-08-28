#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "topdowncamera.h"
#include "bullet.h"
#include "field.h"


void Player::Init()
{
	GameObject::Init();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(1, 1, 1);

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
	GetLookAtDirection();

	// shoot with mouse click
	if (CInput::GetMouseLeftTrigger())
	{
		auto bullet = CManager::GetActiveScene()->AddGameObject<Bullet>(0);
		bullet->SetPosition(m_position);
		bullet->SetDirection(m_lookAtDirection);
	}

	// basic collision with bounds of field
	if (CManager::GetActiveScene()->GetGameObjects<Field>(0).front()->IsOutOfBounds(m_position, 1))
	{
		m_position = m_oldPosition;
	}
}

void Player::Draw()
{
	GameObject::Draw();

	// set the active shader
	CRenderer::SetShader(m_shader);

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

	t._11 = x.x; t._12 = x.y; t._13 = x.z;
	t._21 = y.x; t._22 = y.y; t._23 = y.z;
	t._31 = z.x; t._32 = z.y; t._33 = z.z;

	world = dx::XMLoadFloat4x4(&t);
	m_shader->SetWorldMatrix(&world);

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

void Player::GetLookAtDirection()
{
	// get the mouse coordinate and normalize it from -1 to 1
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(GetWindow(), &point);

	// offset it for camera angle
	point.x += 0;
	point.y += -40;

	float x, z;
	x = (float)(point.x - SCREEN_WIDTH / 2.0F) / SCREEN_WIDTH * 2;
	z = (float)(point.y - SCREEN_HEIGHT / 2.0F) / -SCREEN_HEIGHT * 2;

	dx::XMVECTOR norm = dx::XMVectorSet(x, 0, z,1);
	norm = dx::XMVector3Normalize(norm);
	dx::XMStoreFloat3(&m_lookAtDirection, norm);
}
