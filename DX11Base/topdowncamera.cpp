#include "pch.h"
#include "main.h"
#include "topdowncamera.h"
#include "manager.h"
#include "renderer.h"
#include "shader.h"
#include "player.h"


void TopDownCamera::Init()
{
	Camera::Init();

	m_offset = dx::XMFLOAT3(0, 12, -9);
	m_lerpSpeed = 0.1F;
	m_target = CManager::GetActiveScene()->GetGameObjects<Player>(0).front();
}

void TopDownCamera::Uninit()
{
	Camera::Uninit();
}

void TopDownCamera::Draw(UINT renderPass)
{
	Camera::Draw(renderPass);
}

void TopDownCamera::Update()
{
	Camera::Update();

	if (auto target = m_target.lock())
		m_position = Lerp(m_position, target->GetPosition() + m_offset, m_lerpSpeed);
}

void TopDownCamera::SetViewMatrix()
{
	// get values to create the view matrix
	dx::XMMATRIX view = dx::XMLoadFloat4x4(&m_mView);
	dx::XMVECTOR eye = dx::XMLoadFloat3(&m_position);
	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 1);
	dx::XMVECTOR direction = dx::XMVectorSet(0, -1, 0.9F, 1);

	// calculate and set the view matrix for each shader
	view = dx::XMMatrixLookToLH(eye, direction, up);

	auto shaders = CRenderer::GetShaders();
	for (auto shader : shaders)
		shader->SetViewMatrix(&view);

	// load the view matrix back to member variable
	dx::XMStoreFloat4x4(&m_mView, view);
}
