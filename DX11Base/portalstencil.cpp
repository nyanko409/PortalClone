#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "portalstencil.h"
#include "manager.h"
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
	m_triggerCollider.Init((GameObject*)this, 1.2f, 2, 1.2f);

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
	if ((pass == Pass::PortalBlue && m_type == PortalStencilType::Blue) ||
		(pass == Pass::PortalOrange && m_type == PortalStencilType::Orange))
	{
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
	if ((pass == Pass::PortalBlue && m_type == PortalStencilType::Blue) || 
		(pass == Pass::PortalOrange && m_type == PortalStencilType::Orange))
	{
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
		CRenderer::SetDepthStencilState(4, 0);
		CRenderer::DrawModel(m_shader, m_model, false);

		// draw portal frame
		m_shader->SetValueBuffer(true);
		CRenderer::DrawModel(m_shader, m_model, false);
	}
	else if ((pass == Pass::PortalBlueFrame && m_type == PortalStencilType::Blue) ||
			(pass == Pass::PortalOrangeFrame && m_type == PortalStencilType::Orange))
	{
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

dx::XMMATRIX PortalStencil::GetProjectionMatrix()
{
	if (!Debug::obliqueProjectionEnabled)
		return CManager::GetActiveScene()->GetMainCamera()->GetProjectionMatrix();

	if (auto linkedPortal = m_linkedPortal.lock())
	{
		auto cam = std::static_pointer_cast<FPSCamera>(CManager::GetActiveScene()->GetMainCamera());

		dx::XMFLOAT4X4 v;
		dx::XMStoreFloat4x4(&v, (GetViewMatrix(true)));

		// Find a camera-space position on the plane (it does not matter where on the clip plane, as long as it is on it)
		dx::XMFLOAT3 position;
		dx::XMStoreFloat3(&position, linkedPortal->GetPosition());

		dx::XMFLOAT3 linkedForward = linkedPortal->GetForward();
		position += linkedForward * 0.05f;

		float Px, Py, Pz;
		Px = v._11 * position.x + v._21 * position.y + v._31 * position.z + v._41;
		Py = v._12 * position.x + v._22 * position.y + v._32 * position.z + v._42;
		Pz = v._13 * position.x + v._23 * position.y + v._33 * position.z + v._43;

		// Find the camera-space 4D reflection plane vector
		dx::XMFLOAT3 worldNormal;
		worldNormal.x = linkedForward.x;
		worldNormal.y = linkedForward.y;
		worldNormal.z = linkedForward.z;
		float Cx, Cy, Cz, Cw;
		Cx = v._11 * worldNormal.x + v._21 * worldNormal.y + v._31 * worldNormal.z;
		Cy = v._12 * worldNormal.x + v._22 * worldNormal.y + v._32 * worldNormal.z;
		Cz = v._13 * worldNormal.x + v._23 * worldNormal.y + v._33 * worldNormal.z;
		Cw = -Cx * Px - Cy * Py - Cz * Pz;

		// Modify the projection matrix so that it uses an oblique near clip plane
		dx::XMVECTOR clipPlane = dx::XMVectorSet(Cx, Cy, Cz, Cw);
		return cam->CalculateObliqueMatrix(clipPlane);
	}
}

dx::XMVECTOR PortalStencil::GetClonedVelocity(dx::XMVECTOR velocity) const
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		//direction vector -> in portal local -> rotate locally by y 180 -> out portal world
		velocity = dx::XMVector3TransformNormal(velocity, dx::XMMatrixInverse(nullptr, GetWorldMatrix()));
		velocity = dx::XMVector3TransformNormal(velocity, dx::XMMatrixRotationY(dx::XMConvertToRadians(180)));
		velocity = dx::XMVector3TransformNormal(velocity, linkedPortal->GetWorldMatrix());

		return velocity;
	}

	return dx::XMVECTOR{ 0,0,0 };
}

dx::XMMATRIX PortalStencil::GetClonedOrientationMatrix(dx::XMMATRIX matrix) const
{
	if (auto linkedPortal = m_linkedPortal.lock())
	{
		dx::XMMATRIX out = matrix;
		out *= dx::XMMatrixInverse(nullptr, GetWorldMatrix());
		out *= dx::XMMatrixRotationY(dx::XMConvertToRadians(180));
		out *= linkedPortal->GetWorldMatrix();

		return out;
	}

	return dx::XMMatrixIdentity();
}
