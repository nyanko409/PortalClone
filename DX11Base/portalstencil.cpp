#include "pch.h"
#include "portalstencil.h"
#include "renderer.h"
#include "manager.h"
#include "fpscamera.h"
#include "portalbackfaceshader.h"
#include "debug.h"


void PortalStencil::Awake()
{
	Portal::Awake();

	m_shader = CRenderer::GetShader<PortalStencilShader>();
}

void PortalStencil::Uninit()
{
	GameObject::Uninit();
}

void PortalStencil::Update()
{
	GameObject::Update();

	m_curIteration = 1;
	CRenderer::SetDepthStencilState(1, 0);
}

void PortalStencil::Draw(Pass pass)
{
	if ((pass == Pass::PortalBlue && m_type == PortalType::Blue) ||
		(pass == Pass::PortalOrange && m_type == PortalType::Orange))
	{
		if (!m_linkedPortal.lock())
			return;

		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		m_shader->SetViewMatrix(&GetViewMatrix());
		m_shader->SetProjectionMatrix(&GetProjectionMatrix());

		// draw the portal and adjust the stencil value
		m_shader->SetValueBuffer(false);
		CRenderer::SetDepthStencilState(2, 1);
		m_curIteration++;
		CRenderer::DrawModel(m_shader, m_model, false);
		CRenderer::SetDepthStencilState(0, m_curIteration - 1);
	}
}

void PortalStencil::Draw(const std::shared_ptr<class Shader>& shader, Pass pass)
{
	if ((pass == Pass::PortalBlue && m_type == PortalType::Blue) ||
		(pass == Pass::PortalOrange && m_type == PortalType::Orange))
	{
		if (!m_linkedPortal.lock())
			return;

		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		m_shader->SetValueBuffer(false);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		// draw the portal
		CRenderer::SetDepthStencilState(4, m_curIteration++);
		CRenderer::DrawModel(m_shader, m_model, false);
		CRenderer::SetDepthStencilState(0, m_curIteration - 1);
	}
	else if (pass == Pass::Default)
	{
		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		// draw the portal into depth buffer
		CRenderer::SetDepthStencilState(0, 0);
		m_shader->SetValueBuffer(false);
		CRenderer::DrawModel(m_shader, m_model, false);

		// draw portal frame
		m_shader->SetValueBuffer(true);
		CRenderer::DrawModel(m_shader, m_model, false);

		// draw the colliders
		m_triggerCollider.Draw();
		for (auto col : m_edgeColliders)
			col->Draw();

		CRenderer::SetDepthStencilState(6, 0);
	}
	else if ((pass == Pass::PortalBlueFrame && m_type == PortalType::Blue) ||
			(pass == Pass::PortalOrangeFrame && m_type == PortalType::Orange))
	{
		if (!m_linkedPortal.lock())
			return;

		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		m_shader->SetValueBuffer(true);

		MATERIAL material = {};
		material.Diffuse = m_color;
		m_shader->SetMaterial(material);

		// draw portal frame recursively
		m_curIteration -= 2;
		for (; m_curIteration > 1; m_curIteration--)
		{
			CRenderer::SetDepthStencilState(0, m_curIteration);
			m_shader->SetViewMatrix(&GetViewMatrix());
			m_shader->SetProjectionMatrix(&GetProjectionMatrix());
			CRenderer::DrawModel(m_shader, m_model, false);
		}
	}
	else if (pass == Pass::PortalBackface)
	{
		// draw the portal backface
		MATERIAL material = {};
		material.Diffuse = m_color;
		shader->SetMaterial(material);
		
		dx::XMMATRIX world = GetWorldMatrix();
		shader->SetWorldMatrix(&world);
		
		auto bfs = std::static_pointer_cast<PortalBackfaceShader>(shader);
		
		// stencil pass
		bfs->SetValueBuffer(true);
		CRenderer::SetDepthStencilState(4, 0);
		CRenderer::DrawModel(shader, m_modelPlane, false);
		
		// normal pass
		CRenderer::SetRasterizerState(RasterizerState_CullNone);
		bfs->SetValueBuffer(false);
		CRenderer::SetDepthStencilState(5, 1);
		CRenderer::DrawModel(shader, m_modelPlane, false);
		CRenderer::SetRasterizerState(RasterizerState_CullBack);
	}
}

dx::XMMATRIX PortalStencil::GetViewMatrix(bool firstIteration)
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		// player camera world -> in portal local -> rotate locally by y 180 -> out portal world
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
		int iterationNum = firstIteration ? 0 : m_curIteration - 2;

		// repeat transformation to get the right orientation for current iteration
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
