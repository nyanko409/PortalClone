#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "portal.h"
#include "modelmanager.h"
#include "manager.h"
#include "camera.h"


void Portal::Awake()
{
	GameObject::Awake();

	// get the shader
	m_shader = CRenderer::GetShader<PortalShader>();

	ModelManager::GetModel(MODEL_PORTAL, m_model);

	// init values
	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(2.0F, 2.0F, 2.0F);

	m_enableFrustumCulling = false;
	m_lookAt = { 0,0,-1 };
}

void Portal::Uninit()
{
	GameObject::Uninit();
}

void Portal::Update()
{
	GameObject::Update();
}

void Portal::Draw(Pass pass)
{
	GameObject::Draw(pass);

	if (pass == Pass::Default)
	{
		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		dx::XMFLOAT4X4 t;
		dx::XMStoreFloat4x4(&t, world);
		
		dx::XMVECTOR up = dx::XMLoadFloat3(&m_up);
		dx::XMVECTOR zaxis = dx::XMVector3Normalize(dx::XMLoadFloat3(&m_lookAt));
		dx::XMVECTOR xaxis = dx::XMVector3Normalize(dx::XMVector3Cross(up, zaxis));
		dx::XMVECTOR yaxis = dx::XMVector3Cross(zaxis, xaxis);

		dx::XMFLOAT3 z, x, y;
		dx::XMStoreFloat3(&z, zaxis);
		dx::XMStoreFloat3(&x, xaxis);
		dx::XMStoreFloat3(&y, yaxis);

		t._11 = x.x * m_scale.x;
		t._12 = x.y * m_scale.x;
		t._13 = x.z * m_scale.x;
		t._21 = y.x * m_scale.y;
		t._22 = y.y * m_scale.y;
		t._23 = y.z * m_scale.y;
		t._31 = z.x * m_scale.z;
		t._32 = z.y * m_scale.z;
		t._33 = z.z * m_scale.z;

		world = dx::XMLoadFloat4x4(&t);
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		// draw the model
		CRenderer::DrawModel(m_shader, m_model);
	}
}

dx::XMMATRIX Portal::GetViewMatrix()
{
	dx::XMVECTOR forward = dx::XMLoadFloat3(&m_lookAt);
	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 0);
	dx::XMVECTOR eye = dx::XMVectorSubtract(GetPosition(), forward);

	return dx::XMMatrixLookToLH(eye, forward, up);
}

dx::XMMATRIX Portal::GetProjectionMatrix()
{
	return CManager::GetActiveScene()->GetMainCamera()->GetProjectionMatrix();
}
