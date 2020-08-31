#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "scene.h"
#include "shader.h"


void Camera::Init()
{
	GameObject::Init();

	dx::XMMATRIX view = dx::XMMatrixIdentity();
	dx::XMMATRIX projection = dx::XMMatrixIdentity();

	dx::XMStoreFloat4x4(&m_mView, view);
	dx::XMStoreFloat4x4(&m_mProjection, projection);

	m_nearClip = 0.1F;
	m_farClip = 1000.0F;

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
}

void Camera::Uninit()
{
	GameObject::Uninit();
}

void Camera::Update()
{
	GameObject::Update();
}

void Camera::Draw()
{
	GameObject::Draw();

	SetViewMatrix();
	SetprojectionMatix();
}

void Camera::SetprojectionMatix()
{
	dx::XMMATRIX projection = dx::XMMatrixPerspectiveFovLH(1.0F, (float)SCREEN_WIDTH / SCREEN_HEIGHT, m_nearClip, m_farClip);

	// calculate and set the projection matrix for each shader
	auto shaders = CRenderer::GetShaders();
	for (auto shader : shaders)
	{
		shader->SetProjectionMatrix(&projection);
	}

	// load the projection matrix to member variable
	dx::XMStoreFloat4x4(&m_mProjection, projection);
}
