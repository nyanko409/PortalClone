#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "portal.h"
#include "manager.h"
#include "fpscamera.h"


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
	m_iterationNum = 1;
}

void Portal::Uninit()
{
	GameObject::Uninit();
}

void Portal::Update()
{
	GameObject::Update();

	m_curIteration = m_iterationNum;
}

void Portal::Draw(Pass pass)
{
	GameObject::Draw(pass);

	if (pass == Pass::Default)
	{
		// set buffers
		dx::XMMATRIX world = GetLocalToWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		m_shader->SetValueBuffer(m_linkedPortalActive);
		if(m_linkedPortalActive && m_color.z == 1)
			if (auto texture = m_renderTexture.lock())
				m_shader->SetTexture(texture->GetRenderTexture());
			
		// draw the model
		CRenderer::DrawModel(m_shader, m_model, false);
	}
	else if(m_linkedPortalActive && 
		((pass == Pass::PortalBlueDraw && m_color.z == 1) || (pass == Pass::PortalOrangeDraw && m_color.x == 1)))
	{
		if (pass == Pass::PortalBlueDraw && m_color.z == 1)
		{
			m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Blue));
			m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Blue));
		}
		else if (pass == Pass::PortalOrangeDraw && m_color.x == 1)
		{
			m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Orange));
			m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Orange));
		}

		dx::XMMATRIX world = GetLocalToWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		m_shader->SetValueBuffer(m_linkedPortalActive);
		if (m_linkedPortalActive)
			if (auto texture = m_renderTexture.lock())
				m_shader->SetTexture(texture->GetRenderTexture());

		// draw the model
		CRenderer::DrawModel(m_shader, m_model , false);
	}

	m_curIteration--;
}

dx::XMMATRIX Portal::GetViewMatrix()
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		// player camera world -> in portal local -> rotate locally by y 180 -> out portal world
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

		dx::XMMATRIX out, cam;
		cam = mainCam->GetLocalToWorldMatrix();
		for (int i = 0; i <= 0; ++i)
		{
			out = cam;
			out *= GetWorldToLocalMatrix();
			out *= dx::XMMatrixRotationY(dx::XMConvertToRadians(180));
			out *= linkedPortal->GetLocalToWorldMatrix();
			cam = out;
		}
		
		dx::XMFLOAT4X4 fout;
		dx::XMStoreFloat4x4(&fout, cam);
		
		dx::XMVECTOR forward = dx::XMVectorSet(fout._31, fout._32, fout._33, 0);
		dx::XMVECTOR up = dx::XMVectorSet(fout._21, fout._22, fout._23, 0);
		dx::XMVECTOR eye = dx::XMVectorSet(fout._41, fout._42, fout._43, 1);
		
		return dx::XMMatrixLookToLH(eye, forward, up);
	}
}

dx::XMMATRIX Portal::GetProjectionMatrix()
{
	return CManager::GetActiveScene()->GetMainCamera()->GetProjectionMatrix();
}

dx::XMMATRIX Portal::GetLocalToWorldMatrix()
{
	dx::XMMATRIX world = GetWorldMatrix();
	dx::XMFLOAT4X4 t;
	dx::XMStoreFloat4x4(&t, world);

	dx::XMVECTOR forward = dx::XMLoadFloat3(&m_lookAt);
	dx::XMVECTOR up = dx::XMLoadFloat3(&m_up);

	dx::XMVECTOR zaxis = dx::XMVector3Normalize(forward);
	dx::XMVECTOR yaxis = dx::XMVector3Normalize(up);
	dx::XMVECTOR xaxis = dx::XMVector3Cross(yaxis, zaxis);

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

	return dx::XMLoadFloat4x4(&t);
}

dx::XMMATRIX Portal::GetWorldToLocalMatrix()
{
	return dx::XMMatrixInverse(nullptr, GetLocalToWorldMatrix());
}
