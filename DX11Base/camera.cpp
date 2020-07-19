#include "pch.h"
#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "scene.h"


void Camera::Init()
{
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMMATRIX projection = dx::XMLoadFloat4x4(&m_mProjection);

	view = dx::XMMatrixIdentity();
	projection = dx::XMMatrixIdentity();

	dx::XMStoreFloat4x4(&m_mView, view);
	dx::XMStoreFloat4x4(&m_mProjection, projection);

	m_perspective = true;
	m_position = dx::XMFLOAT3(0.0F, 2.0F, -5.0F);
	m_target = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);

	m_forward = dx::XMFLOAT3(0, 0, 1);
	m_right = dx::XMFLOAT3(1, 0, 0 );
}

void Camera::Uninit()
{

}

void Camera::Update()
{
	
}

void Camera::Draw()
{
	if (m_perspective)
	{
		SetViewMatrix();
		SetprojectionMatix();
	}
	else
		Set2DProjection();
}

void Camera::SetViewMatrix()
{
	// convert float4x4 to matrices
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMVECTOR eye = dx::XMLoadFloat3(&m_position);
	dx::XMVECTOR forward = dx::XMLoadFloat3(&m_forward);

	dx::XMFLOAT3 fup = dx::XMFLOAT3(0, 1, 0);
	dx::XMVECTOR up = dx::XMLoadFloat3(&fup);

	// calculate and set the view matrix
	view = dx::XMMatrixLookAtLH(eye, dx::XMVectorAdd(eye, forward), up);

	CRenderer::SetViewMatrix(&view);

	// load the view matrix back to member variable
	dx::XMStoreFloat4x4(&m_mView, view);
}

void Camera::SetprojectionMatix()
{
	dx::XMMATRIX projection = dx::XMLoadFloat4x4(&m_mProjection);

	projection = dx::XMMatrixPerspectiveFovLH(1.0F, (float)SCREEN_WIDTH / SCREEN_HEIGHT, m_nearClip, m_farClip);

	CRenderer::SetProjectionMatrix(&projection);

	dx::XMStoreFloat4x4(&m_mProjection, projection);
}

void Camera::Set2DProjection()
{
	// set world matrix
	dx::XMMATRIX world;
	world = dx::XMMatrixIdentity();
	CRenderer::SetWorldMatrix(&world);

	// set view matrix
	dx::XMMATRIX view = dx::XMMatrixIdentity();
	CRenderer::SetViewMatrix(&view);
	dx::XMStoreFloat4x4(&m_mView, view);

	// set projection matrix
	dx::XMMATRIX projection = dx::XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	CRenderer::SetProjectionMatrix(&projection);
	dx::XMStoreFloat4x4(&m_mProjection, projection);
}
