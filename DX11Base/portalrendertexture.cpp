#include "pch.h"
#include "portalrendertexture.h"
#include "renderer.h"
#include "manager.h"
#include "fpscamera.h"
#include "debug.h"


void PortalRenderTexture::Awake()
{
	Portal::Awake();

	// get the shader
	m_shader = CRenderer::GetShader<PortalRenderTextureShader>();
}

void PortalRenderTexture::Uninit()
{
	GameObject::Uninit();
}

void PortalRenderTexture::Update()
{
	GameObject::Update();

	m_curIteration = m_iterationNum;
	SetupNextIteration();
}

void PortalRenderTexture::Draw(const std::shared_ptr<class Shader>& shader, Pass pass)
{
	if (pass == Pass::StencilOnly)
	{
		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		shader->SetWorldMatrix(&world);

		// draw the model
		CRenderer::SetDepthStencilState(2, 1);
		CRenderer::DrawModel(shader, m_model, false);
		CRenderer::SetDepthStencilState(1, 0);
	}
}

void PortalRenderTexture::Draw(Pass pass)
{
	GameObject::Draw(pass);

	// draw the portals with the recursive rendered texture
	if (pass == Pass::Default)
	{
		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		bool active = m_linkedPortal.lock() ? true : false;
		if (Debug::cameraNum == 1 || Debug::cameraNum == 2)
			active = false;

		m_shader->SetValueBuffer(active);
		if(active)
			if (auto texture = m_activeRenderTexture.lock())
				m_shader->SetTexture(texture->GetRenderTexture());
			
		// draw the model
		CRenderer::SetRasterizerState(RasterizerState::RasterizerState_CullNone);
		CRenderer::DrawModel(m_shader, m_model, false);
		CRenderer::SetRasterizerState(RasterizerState::RasterizerState_CullBack);

		// draw the colliders
		m_triggerCollider.Draw();
		for (auto col : m_edgeColliders)
			col->Draw();
	}

	// draw the view from portal recursively into render texture
	else if(m_linkedPortal.lock() && ((pass == Pass::PortalBlue && m_type == PortalType::Blue) || (pass == Pass::PortalOrange && m_type == PortalType::Orange)))
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

			// move the portal a little forward to prevent z fighting
			dx::XMMATRIX world = GetWorldMatrix();
			dx::XMFLOAT3 forward = GetForward();
			world *= dx::XMMatrixTranslation(forward.x * 0.02f, forward.y * 0.02f, forward.z * 0.02f);
			m_shader->SetWorldMatrix(&world);

			MATERIAL material = {};
			material.Diffuse = m_color;
			m_shader->SetMaterial(material);

			bool active = m_linkedPortal.lock() ? true : false;
			if (Debug::cameraNum == 1 || Debug::cameraNum == 2)
				active = false;

			m_shader->SetValueBuffer(active);
			if (active)
				if (auto texture = m_activeRenderTexture.lock())
					m_shader->SetTexture(texture->GetRenderTexture());

			// draw the model
			CRenderer::DrawModel(m_shader, m_model, false);
		}

		m_curIteration--;
		SetupNextIteration();
	}
}

dx::XMMATRIX PortalRenderTexture::GetViewMatrix(bool firstIteration)
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		// player camera world -> in portal local -> rotate locally by y 180 -> out portal world
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
		int iterationNum = firstIteration ? 0 : m_curIteration;

		// repeat transformation to get the right position for current iteration
		dx::XMMATRIX out, cam;
		cam = mainCam->GetLocalToWorldMatrix(false);
		for (int i = 0; i <= iterationNum; ++i)
		{
			out = cam;
			out *= dx::XMMatrixInverse(nullptr, GetWorldMatrix());
			out *= dx::XMMatrixRotationY(dx::XMConvertToRadians(180));
			out *= linkedPortal->GetWorldMatrix();
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

void PortalRenderTexture::SetupNextIteration()
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
