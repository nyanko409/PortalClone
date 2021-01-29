#include "pch.h"
#include "portalstencil.h"
#include "renderer.h"
#include "manager.h"
#include "modelmanager.h"
#include "fpscamera.h"
#include "debug.h"


void PortalStencil::Awake()
{
	GameObject::Awake();

	m_shader = CRenderer::GetShader<PortalStencilShader>();
	ModelManager::GetModel(MODEL_PORTAL, m_model);

	// init values
	SetPosition(0.0F, 0.0F, 0.0F);
	SetRotation(0.0F, 0.0F, 0.0F);
	SetScale(2.0F, 2.0F, 2.0F);

	m_enableFrustumCulling = false;

	// colliders
	m_triggerCollider.Init((GameObject*)this, 1.2f, 2.0f, 1.2f);

	m_edgeColliders = std::vector<OBB*>();

	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 0.4f, 3, 2, 1.2f, 0, -0.99f);
	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 0.4f, 3, 2, -1.2f, 0, -0.99f);

	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 3, 0.4f, 2, 0, 2.1f, -0.99f);
	//m_edgeColliders.push_back(new OBB());
	//m_edgeColliders.back()->Init((GameObject*)this, 3, 0.4f, 2, 0, -2.1f, -0.99f);
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

	m_triggerCollider.Update();
	for (auto col : m_edgeColliders)
		col->Update();
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

		// draw the portal
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
		m_shader->SetValueBuffer(false);
		CRenderer::SetDepthStencilState(0, 0);
		CRenderer::DrawModel(m_shader, m_model, false);

		// draw portal frame
		m_shader->SetValueBuffer(true);
		CRenderer::DrawModel(m_shader, m_model, false);

		// draw the colliders
		m_triggerCollider.Draw();
		for (auto col : m_edgeColliders)
			col->Draw();
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
}

dx::XMMATRIX PortalStencil::GetViewMatrix(bool firstIteration)
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		// player camera world -> in portal local -> rotate locally by y 180 -> out portal world
		auto mainCam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());
		int iterationNum = firstIteration ? 0 : m_curIteration - 2;

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
