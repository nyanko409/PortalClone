#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "scene.h"
#include "bullet.h"
#include "billboard.h"


void Camera::Init()
{
	D3DXMatrixIdentity(&m_mView);
	D3DXMatrixIdentity(&m_mProjection);

	m_perspective = true;
	m_position = D3DXVECTOR3(0.0F, 2.0F, -5.0F);
	m_target = D3DXVECTOR3(0.0F, 0.0F, 0.0F);

	m_forward = { 0, 0, 1 };
	m_right = { 1, 0, 0 };
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
	D3DXMatrixLookAtLH(&m_mView, &m_position, &(m_position + m_forward), &D3DXVECTOR3(0.0F, 1.0F, 0.0F));
	CRenderer::SetViewMatrix(&m_mView);
}

void Camera::SetprojectionMatix()
{
	D3DXMatrixPerspectiveFovLH(&m_mProjection, 1.0F, (float)SCREEN_WIDTH / SCREEN_HEIGHT, m_nearClip, m_farClip);
	CRenderer::SetProjectionMatrix(&m_mProjection);
}

void Camera::Set2DProjection()
{
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	CRenderer::SetWorldMatrix(&world);

	D3DXMatrixIdentity(&m_mView);
	CRenderer::SetViewMatrix(&m_mView);

	D3DXMatrixOrthoOffCenterLH(&m_mProjection, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	CRenderer::SetProjectionMatrix(&m_mProjection);
}
