#include "pch.h"
#include "main.h"
#include "topdowncamera.h"
#include "manager.h"
#include "renderer.h"
#include "shader.h"


void TopDownCamera::Init()
{
	Camera::Init();

	m_offset = dx::XMFLOAT3(0, 20, -9);
	m_lerpSpeed = 0.1F;
}

void TopDownCamera::Uninit()
{
	Camera::Uninit();
}

void TopDownCamera::Draw()
{
	Camera::Draw();
}

void TopDownCamera::Update()
{
	Camera::Update();

	if (m_target)
		m_position = Lerp(m_position, m_target->GetPosition() + m_offset, m_lerpSpeed);
}

void TopDownCamera::SetViewMatrix()
{
	// get values to create the view matrix
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMVECTOR eye = dx::XMLoadFloat3(&m_position);
	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 1);
	dx::XMVECTOR direction = dx::XMVectorSet(0, -1, 0.4F, 1);

	// calculate and set the view matrix for each shader
	view = dx::XMMatrixLookToLH(eye, direction, up);

	auto shaders = CRenderer::GetShaders();
	for (Shader* shader : shaders)
		shader->SetViewMatrix(&view);

	// load the view matrix back to member variable
	dx::XMStoreFloat4x4(&m_mView, view);
}
