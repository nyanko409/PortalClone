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
#include "terrain.h"


void Player::Init()
{
	GameObject::Init();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_PLAYER, m_model);

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(0.02F, 0.02F, 0.02F);

	m_obb.Init((GameObject*)this, 200, 200, 200);
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
	m_obb.Update();

	static float frame = 0;
	frame += .5F;

	if (CInput::GetKeyPress(DIK_2))
		m_model->Update(frame, 1);
	else if (CInput::GetKeyPress(DIK_3))
		m_model->Update(frame, 2);
	else
		m_model->Update(frame, 0);

	Movement();

	auto enemy = CManager::GetActiveScene()->GetGameObjects<Enemy>(0).front();
	m_colliding = m_obb.CheckObbCollision(&enemy->obb);
}

void Player::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	if (renderPass == 1)
	{
		// lighting
		LIGHT light;
		light.Enable = true;
		light.Direction = dx::XMFLOAT4(0.5F, -1.0F, 0.0F, 0.0F);
		dx::XMStoreFloat4(&light.Direction, dx::XMVector4Normalize(dx::XMLoadFloat4(&light.Direction)));

		light.Ambient = dx::XMFLOAT4(.1F, .1F, .1F, 1.0F);
		light.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
		m_shader->SetLight(light);

		// set the world matrix
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		// draw the model
		CRenderer::DrawModel(m_shader, m_model);

		m_obb.Draw();

		ImGui::SetNextWindowSize(ImVec2(150, 200));
		ImGui::Begin("Player Debug");
		ImGui::Checkbox("is colliding", &m_colliding);
		ImGui::End();
	}
	else if (renderPass == 2)
	{
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
		m_shader->SetWorldMatrix(&world);

		// top-down minimap view angle
		dx::XMMATRIX view;
		dx::XMVECTOR eye = dx::XMVectorSet(0, 20, 0, 1);
		eye += m_position;

		dx::XMVECTOR forward = dx::XMVectorSet(0, -1, 0.0F, 1);

		dx::XMFLOAT3 fup = dx::XMFLOAT3(0, 0, 1);
		dx::XMVECTOR up = dx::XMLoadFloat3(&fup);

		view = dx::XMMatrixLookAtLH(eye, dx::XMVectorAdd(eye, forward), up);
		m_shader->SetViewMatrix(&view);

		CRenderer::DrawModel(m_shader, m_model);
	}
}

void Player::Movement()
{
	// normalized wasd movement
	dx::XMVECTOR moveDirection = dx::XMVectorZero();
	if (CInput::GetKeyPress(DIK_W))
	{
		moveDirection += {0, 0, 1};
	}
	if (CInput::GetKeyPress(DIK_A))
	{
		moveDirection -= {1, 0, 0};
	}
	if (CInput::GetKeyPress(DIK_S))
	{
		moveDirection -= {0, 0, 1};
	}
	if (CInput::GetKeyPress(DIK_D))
	{ 
		moveDirection += {1, 0, 0};
	}
	if (CInput::GetKeyPress(DIK_Q))
	{
		m_rotation.y -= 0.3F;
	}
	if (CInput::GetKeyPress(DIK_E))
	{
		m_rotation.y += 0.3F;
	}

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

void Player::ShootProjectile()
{
	auto bullet = CManager::GetActiveScene()->AddGameObject<Bullet>(0);
	bullet->SetPosition(m_position);
	bullet->SetDirection(m_lookAtDirection);
}
