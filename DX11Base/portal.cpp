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
	m_obb.Init((GameObject*)this, 1, 1, 1);
}

void Portal::Uninit()
{
	GameObject::Uninit();
}

void Portal::Update()
{
	GameObject::Update();

	m_curIteration = m_iterationNum;
	SetupNextIteration();
	m_obb.Update();
}

void Portal::Draw(Pass pass)
{
	GameObject::Draw(pass);

	// draw the portals with the recursive rendered texture
	if (pass == Pass::Default)
	{
		// set buffers
		dx::XMMATRIX world = GetLocalToWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		m_shader->SetValueBuffer(m_linkedPortalActive);
		if(m_linkedPortalActive)
			if (auto texture = m_activeRenderTexture.lock())
				m_shader->SetTexture(texture->GetRenderTexture());
			
		// draw the model
		CRenderer::SetRasterizerState(RasterizerState::RasterizerState_CullNone);
		CRenderer::DrawModel(m_shader, m_model, false);
		CRenderer::SetRasterizerState(RasterizerState::RasterizerState_CullBack);
		m_obb.Draw();
	}

	// draw the view from portal recursively into render texture
	else if(m_linkedPortalActive && ((pass == Pass::PortalBlue && m_type == PortalType::Blue) || (pass == Pass::PortalOrange && m_type == PortalType::Orange)))
	{
		// skip last iteration to prevent drawing a black portal
		if (m_curIteration != m_iterationNum)
		{
			if (pass == Pass::PortalBlue && m_type == PortalType::Blue)
			{
				m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Blue));
				m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Blue));
			}
			else if (pass == Pass::PortalOrange && m_type == PortalType::Orange)
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
				if (auto texture = m_activeRenderTexture.lock())
					m_shader->SetTexture(texture->GetRenderTexture());

			// draw the model
			CRenderer::DrawModel(m_shader, m_model, false);
		}

		m_curIteration--;
		SetupNextIteration();
	}
}

dx::XMMATRIX Portal::GetViewMatrix(bool firstIteration)
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		// player camera world -> in portal local -> rotate locally by y 180 -> out portal world
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
		int iterationNum = firstIteration ? 0 : m_curIteration;

		dx::XMMATRIX out, cam;
		cam = mainCam->GetLocalToWorldMatrix();
		for (int i = 0; i <= iterationNum; ++i)
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
	//return CManager::GetActiveScene()->GetMainCamera()->GetProjectionMatrix();

	if (auto linkedPortal = m_linkedPortal.lock())
	{
		auto cam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

		//dx::XMVECTOR clipPlane = dx::XMVectorSet(linkedPortal->m_lookAt.x, linkedPortal->m_lookAt.y, linkedPortal->m_lookAt.z, dx::XMVectorGetX(dx::XMVector3Length(linkedPortal->GetPosition())));
		//clipPlane = dx::XMVector4Transform(clipPlane, dx::XMMatrixTranspose(dx::XMMatrixInverse(nullptr, GetViewMatrix())));

		//dx::XMMATRIX m = GetViewMatrix(true);
		//dx::XMVECTOR cpos = dx::XMVector3TransformCoord(linkedPortal->GetPosition(), m);
		//dx::XMVECTOR point = dx::XMVector3TransformCoord(dx::XMVectorSet(0,0,0,1), dx::XMMatrixInverse(nullptr, m));
		//cpos = dx::XMVectorSubtract(cpos, dx::XMVectorSet(0, dx::XMVectorGetY(point), 0, 0));
		//dx::XMVECTOR cnormal = dx::XMVector3Normalize(dx::XMVector3Transform(dx::XMLoadFloat3(&linkedPortal->m_lookAt), m));
		//dx::XMVECTOR clipPlane = cnormal;
		//clipPlane = dx::XMVectorSetW(clipPlane, -dx::XMVectorGetX(dx::XMVector3Dot(cpos, cnormal)));
		//
		//return cam->CalculateObliqueMatrix(clipPlane);

		//Get the current view and projection matrices
		dx::XMFLOAT4X4 v;
		dx::XMStoreFloat4x4(&v, (GetViewMatrix(true)));

		//Find a camera-space position on the plane (it does not matter where on the clip plane, as long as it is on it)
		//Wx, Wy, Wz is the world-space plane coordinate
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, linkedPortal->GetPosition());
		float Px, Py, Pz;
		Px = v._11 * position.x + v._21 * position.y + v._31 * position.z + v._41;
		Py = v._12 * position.x + v._22 * position.y + v._32 * position.z + v._42;
		Pz = v._13 * position.x + v._23 * position.y + v._33 * position.z + v._43;

		//Find the camera-space 4D reflection plane vector
		//Nx, Ny, Nz is the world-space normal of the plane
		dx::XMFLOAT3 worldNormal;
		worldNormal.x = linkedPortal->m_lookAt.x + position.x;
		worldNormal.y = linkedPortal->m_lookAt.y + position.y;
		worldNormal.z = linkedPortal->m_lookAt.z + position.z;
		float Cx, Cy, Cz, Cw;
		Cx = v._11 * worldNormal.x + v._21 * worldNormal.y + v._31 * worldNormal.z;
		Cy = v._12 * worldNormal.x + v._22 * worldNormal.y + v._32 * worldNormal.z;
		Cz = v._13 * worldNormal.x + v._23 * worldNormal.y + v._33 * worldNormal.z;
		Cw = -(Cx * Px - Cy * Py - Cz * Pz);

		//Modify the projection matrix so that it uses an oblique near clip plane
		dx::XMVECTOR clipPlane = dx::XMVectorSet(Cx, Cy, Cz, Cw);
		return cam->CalculateObliqueMatrix(clipPlane);
	}
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

dx::XMMATRIX Portal::GetPlayerOrientationMatrix(dx::XMFLOAT3 position)
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

		dx::XMVECTOR xaxis = dx::XMVector3Normalize(mainCam->GetRightVector());
		dx::XMVECTOR zaxis = dx::XMVector3Normalize(mainCam->GetForwardVector());
		dx::XMVECTOR yaxis = dx::XMVector3Cross(zaxis, xaxis);

		dx::XMFLOAT3 z, x, y;
		dx::XMStoreFloat3(&z, zaxis);
		dx::XMStoreFloat3(&x, xaxis);
		dx::XMStoreFloat3(&y, yaxis);

		// master object world -> in portal local -> rotate locally by y 180 -> out portal world
		dx::XMFLOAT4X4 camOut;
		dx::XMStoreFloat4x4(&camOut, mainCam->GetLocalToWorldMatrix());

		camOut._11 = x.x;
		camOut._12 = x.y;
		camOut._13 = x.z;
		camOut._21 = y.x;
		camOut._22 = y.y;
		camOut._23 = y.z;
		camOut._31 = z.x;
		camOut._32 = z.y;
		camOut._33 = z.z;
		camOut._41 = position.x;
		camOut._42 = position.y;
		camOut._43 = position.z;

		dx::XMMATRIX out = dx::XMLoadFloat4x4(&camOut);
		out *= GetWorldToLocalMatrix();
		out *= dx::XMMatrixRotationY(dx::XMConvertToRadians(180));
		out *= linkedPortal->GetLocalToWorldMatrix();

		return out;
	}

	return dx::XMMatrixIdentity();
}

dx::XMMATRIX Portal::GetPlayerWorldMatrix(dx::XMFLOAT3 position)
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

		dx::XMVECTOR xaxis = dx::XMVector3Normalize(mainCam->GetRightVector());
		dx::XMVECTOR zaxis = dx::XMVector3Normalize(dx::XMVectorSetY(mainCam->GetForwardVector(), 0));
		dx::XMVECTOR yaxis = dx::XMVector3Cross(zaxis, xaxis);

		dx::XMFLOAT3 z, x, y;
		dx::XMStoreFloat3(&z, zaxis);
		dx::XMStoreFloat3(&x, xaxis);
		dx::XMStoreFloat3(&y, yaxis);

		// master object world -> in portal local -> rotate locally by y 180 -> out portal world
		dx::XMFLOAT4X4 camOut;
		dx::XMStoreFloat4x4(&camOut, mainCam->GetLocalToWorldMatrix());

		camOut._11 = x.x;
		camOut._12 = x.y;
		camOut._13 = x.z;
		camOut._21 = y.x;
		camOut._22 = y.y;
		camOut._23 = y.z;
		camOut._31 = z.x;
		camOut._32 = z.y;
		camOut._33 = z.z;
		camOut._41 = position.x;
		camOut._42 = position.y;
		camOut._43 = position.z;

		dx::XMMATRIX out = dx::XMLoadFloat4x4(&camOut);
		out *= GetWorldToLocalMatrix();
		out *= dx::XMMatrixRotationY(dx::XMConvertToRadians(180));
		out *= linkedPortal->GetLocalToWorldMatrix();

		return out;
	}

	return dx::XMMatrixIdentity();
}

void Portal::SetupNextIteration()
{
	if (m_iterationNum % 2 == 0)
	{
		if (m_curIteration % 2 == 0)
		{
			if (auto texture = m_tempRenderTexture.lock())
				m_activeRenderTexture = texture;
		}
		else
		{
			if (auto texture = m_renderTexture.lock())
				m_activeRenderTexture = texture;
		}
	}
	else
	{
		if (m_curIteration % 2 == 0)
		{
			if (auto texture = m_renderTexture.lock())
				m_activeRenderTexture = texture;
		}
		else
		{
			if (auto texture = m_tempRenderTexture.lock())
				m_activeRenderTexture = texture;
		}
	}
}
