#include "pch.h"
#include "camera.h"
#include "main.h"
#include "renderer.h"
#include "shader.h"
#include "frustumculling.h"
#include "portalmanager.h"


void Camera::Init()
{
	GameObject::Init();

	dx::XMMATRIX view = dx::XMMatrixIdentity();
	dx::XMMATRIX projection = dx::XMMatrixIdentity();

	dx::XMStoreFloat4x4(&m_mView, view);
	dx::XMStoreFloat4x4(&m_mProjection, projection);

	m_nearClip = 0.05F;
	m_farClip = 300.0F;
	m_debugCameraNum = 0;

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

void Camera::Draw(Pass pass)
{
	GameObject::Draw(pass);

	if (m_debugCameraNum == 0)
	{
		SetViewMatrix();
		SetProjectionMatix();
		SetCameraPositionBuffers();
	}
	else if (m_debugCameraNum == 1 || m_debugCameraNum == 2)
	{
		// view, projection
		auto portal = PortalManager::GetPortal(m_debugCameraNum == 1 ? PortalType::Blue : PortalType::Orange);
		dx::XMMATRIX view = portal->GetViewMatrix(true);
		dx::XMMATRIX projection = portal->GetProjectionMatrix();
		auto shaders = CRenderer::GetShaders();
		for (auto shader : shaders)
		{
			shader->SetViewMatrix(&view);
			shader->SetProjectionMatrix(&projection);
		}
	}
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

void Camera::SetCameraPositionBuffers()
{
	auto shaders = CRenderer::GetShaders();
	for (auto shader : shaders)
	{
		shader->SetCameraPosition(&m_position);
	}
}

dx::XMMATRIX Camera::CalculateObliqueMatrix(dx::XMVECTOR clipPlane)
{
	dx::XMFLOAT4X4 matrix;
	dx::XMVECTOR q = dx::XMVectorSet(0, 0, 0, 0);

	dx::XMStoreFloat4x4(&matrix, GetProjectionMatrix());

	// Calculate the clip-space corner point opposite the clipping plane
	// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
	// transform it into camera space by multiplying it
	// by the inverse of the projection matrix
	q = dx::XMVectorSetX(q, (Sign(dx::XMVectorGetX(clipPlane)) + matrix._31) / matrix._11);
	q = dx::XMVectorSetY(q, (Sign(dx::XMVectorGetY(clipPlane)) + matrix._32) / matrix._22);
	q = dx::XMVectorSetZ(q, 1.0f);
	q = dx::XMVectorSetW(q, (1.0f + matrix._33) / matrix._43);

	// Calculate the scaled plane vector
	dx::XMVECTOR c = dx::XMVectorScale(clipPlane, (2.0f / dx::XMVectorGetX(dx::XMVector4Dot(clipPlane, q))));

	// Replace the third row of the projection matrix
	matrix._13 = dx::XMVectorGetX(c);
	matrix._23 = dx::XMVectorGetY(c);
	matrix._33 = dx::XMVectorGetZ(c);
	matrix._43 = dx::XMVectorGetW(c);

	return dx::XMLoadFloat4x4(&matrix);
}
