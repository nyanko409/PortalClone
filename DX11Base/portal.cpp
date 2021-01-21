#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "portal.h"
#include "manager.h"
#include "fpscamera.h"
#include "debug.h"


void Portal::Awake()
{
	GameObject::Awake();

	// get the shader
	m_shader = CRenderer::GetShader<PortalShader>();

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
	m_edgeColliders.back()->Init((GameObject*)this, 0.4f, 3, 2, 1.2f, 0, -1.0f);
	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 0.4f, 3, 2, -1.2f, 0, -1.0f);

	m_edgeColliders.push_back(new OBB());
	m_edgeColliders.back()->Init((GameObject*)this, 3, 0.4f, 2, 0, 2.1f, -1.0f);
	//m_edgeColliders.push_back(new OBB());
	//m_edgeColliders.back()->Init((GameObject*)this, 3, 0.4f, 2, 0, -2.1f, 1.0f);
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

	m_triggerCollider.Update();
	for (auto col : m_edgeColliders)
		col->Update();
}

void Portal::Draw(const std::shared_ptr<class Shader>& shader, Pass pass)
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

void Portal::Draw(Pass pass)
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

		m_shader->SetValueBuffer(m_linkedPortalActive);
		if(m_linkedPortalActive)
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

			// move the portal a little forward to prevent z fighting
			dx::XMMATRIX world = GetWorldMatrix();
			dx::XMFLOAT3 forward = GetForward();
			world *= dx::XMMatrixTranslation(forward.x * 0.02f, forward.y * 0.02f, forward.z * 0.02f);
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

dx::XMMATRIX Portal::GetProjectionMatrix()
{
	if(!Debug::obliqueProjectionEnabled)
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

dx::XMVECTOR Portal::GetClonedVelocity(dx::XMVECTOR velocity) const
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

dx::XMMATRIX Portal::GetClonedOrientationMatrix(dx::XMMATRIX matrix) const
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
