#include "pch.h"
#include "camera.h"
#include "main.h"
#include "renderer.h"
#include "shader.h"
#include "frustumculling.h"


void Camera::Init()
{
	GameObject::Init();

	dx::XMMATRIX view = dx::XMMatrixIdentity();
	dx::XMMATRIX projection = dx::XMMatrixIdentity();

	dx::XMStoreFloat4x4(&m_mView, view);
	dx::XMStoreFloat4x4(&m_mProjection, projection);

	m_nearClip = 0.1F;
	m_farClip = 150.0F;

	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
}

void Camera::Uninit()
{
	GameObject::Uninit();
}

void Camera::Update()
{
	GameObject::Update();

	FrustumCulling::ConstructFrustum(m_farClip, GetProjectionMatrix(), GetViewMatrix());
}

void Camera::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	SetViewMatrix();
	SetProjectionMatix();
}

void Camera::SetProjectionMatix()
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
