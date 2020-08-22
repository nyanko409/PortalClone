#include "pch.h"
#include "manager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "topdowncamera.h"

dx::XMFLOAT3 g_test;

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

	m_rotation.y += 1;
	Movement();
	MouseRaycast();
}

void Player::Draw()
{
	GameObject::Draw();

	// set the active shader
	CRenderer::SetShader(m_shader);

	// set the world matrix for this object
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	// draw the model
	m_model->Draw(m_shader);

	ImGui::SetNextWindowSize(ImVec2(320, 100));
	ImGui::Begin("Hello World");
	ImGui::Text("X: %f \n Y: %f \n Z: %f", g_test.x, g_test.y, g_test.z);
	ImGui::End();
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

void Player::MouseRaycast()
{
	POINT point;
	GetCursorPos(&point);

	auto cam = CManager::GetActiveScene()->GetGameObjects<TopDownCamera>(0).front();

	dx::XMVECTOR mouseNear = dx::XMVectorSet((float)point.x, (float)point.y, 0.0f, 0.0f);
	dx::XMVECTOR mouseFar = dx::XMVectorSet((float)point.x, (float)point.y, 1.0f, 0.0f);

	dx::XMVECTOR unprojectedNear = dx::XMVector3Unproject(mouseNear, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0F, 1.0F,
		cam->GetProjectionMatrix(), cam->GetViewMatrix(), dx::XMMatrixIdentity());
	dx::XMVECTOR unprojectedFar = dx::XMVector3Unproject(mouseFar, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0F, 1.0F,
		cam->GetProjectionMatrix(), cam->GetViewMatrix(), dx::XMMatrixIdentity());

	dx::XMVECTOR result = dx::XMVector3Normalize(DirectX::XMVectorSubtract(unprojectedFar, unprojectedNear));

	dx::XMFLOAT3 direction;
	dx::XMStoreFloat3(&direction, result);

	dx::XMStoreFloat3(&g_test, unprojectedNear);
}
