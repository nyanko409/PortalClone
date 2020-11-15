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
	dx::XMVECTOR direction = dx::XMVectorSet(0, -0.7F, 0.9F, 1);

	// calculate and set the view matrix for each shader
	view = dx::XMMatrixLookToLH(eye, direction, up);

	auto shaders = CRenderer::GetShaders();
	for (auto shader : shaders)
		shader->SetViewMatrix(&view);

	// load the view matrix back to member variable
	dx::XMStoreFloat4x4(&m_mView, view);
}

bool TopDownCamera::InFrustum(dx::XMFLOAT3 position)
{
	// get the inverse of view projection
	dx::XMMATRIX vp, invvp;
	vp = dx::XMLoadFloat4x4(&m_mView) * dx::XMLoadFloat4x4(&m_mProjection);
	invvp = dx::XMMatrixInverse(nullptr, vp);

	dx::XMVECTOR vpos[4];
	dx::XMVECTOR wpos[4];

	vpos[0] = dx::XMVectorSet(-1.0F, 1.0F, 1.0F, 1.0F);
	vpos[1] = dx::XMVectorSet(1.0F,  1.0F, 1.0F, 1.0F);
	vpos[2] = dx::XMVectorSet(-1.0F, -1.0F, 1.0F, 1.0F);
	vpos[3] = dx::XMVectorSet(1.0F,  -1.0F, 1.0F, 1.0F);

	wpos[0] = dx::XMVector3TransformCoord(vpos[0], invvp);
	wpos[1] = dx::XMVector3TransformCoord(vpos[1], invvp);
	wpos[2] = dx::XMVector3TransformCoord(vpos[2], invvp);
	wpos[3] = dx::XMVector3TransformCoord(vpos[3], invvp);

	// get the needed direction vectors
	dx::XMVECTOR v, v1, v2, n;
	dx::XMVECTOR objPos, camPos;

	objPos = dx::XMLoadFloat3(&position);
	camPos = dx::XMLoadFloat3(&m_position);

	v = dx::XMVectorSubtract(objPos, camPos);

	// check the left frustum
	v1 = dx::XMVectorSubtract(wpos[0], camPos);
	v2 = dx::XMVectorSubtract(wpos[2], camPos);
	n = dx::XMVector3Cross(v1, v2);

	if (dx::XMVectorGetX(dx::XMVector3Dot(n, v)) < 0.0F)
		return false;

	// check the right frustum
	v1 = dx::XMVectorSubtract(wpos[1], camPos);
	v2 = dx::XMVectorSubtract(wpos[3], camPos);
	n = dx::XMVector3Cross(v1, v2);

	if (dx::XMVectorGetX(dx::XMVector3Dot(n, v)) > 0.0F)
		return false;

	return true;
}
