#include "pch.h"
#include "manager.h"
#include "modelmanager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "cube.h"
#include "input.h"
#include "main.h"
#include "stage.h"
#include "light.h"
#include "rendertexture.h"


void Cube::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_CUBE, m_model);

	SetPosition(0.0F, 1.2F, 10.0F);
	SetRotation(0, 0, 0);
	m_scale = dx::XMFLOAT3(0.15F, 0.15F, 0.15F);

	m_entrancePortal = PortalType::None;
	m_isGrounded = true;
	m_enableFrustumCulling = false;
	m_velocity = { 0,0,0 };

	m_obb.Init((GameObject*)this, 11.0f, 11.0f, 11.0f, 0, 0, 0);
}

void Cube::Init()
{
	GameObject::Init();

	PortalManager::AddPortalTraveler(CManager::GetActiveScene()->GetSharedPointer(0, this));
}

void Cube::Uninit()
{
	GameObject::Uninit();
}

void Cube::Update()
{
	GameObject::Update();

	PortalFunneling();

	// apply gravity
	m_velocity.y -= 0.02f;

	// clamp velocity
	m_velocity.y = Clamp(-1.0f, 10.0f, m_velocity.y);

	// update position
	AddPosition(m_velocity);

	// reduce velocity over time to 0 because of portal velocity
	if(m_isGrounded)
		m_velocity = Lerp(m_velocity, dx::XMFLOAT3{ 0,m_velocity.y,0 }, 0.1f);

	// collision
	UpdateCollision();
}

void Cube::UpdateCollision()
{
	float startY = m_position.y;
	auto portal = PortalManager::GetPortal(m_entrancePortal);

	// portal collision
	if (portal)
	{
		auto colliders = portal->GetEdgeColliders();
		for (auto col : *colliders)
		{
			AddPosition(Collision::ObbObbCollision(&m_obb, col));
		}
	}

	// stage collision
	auto stageColliders = CManager::GetActiveScene()->GetGameObjectsOfType<Stage>(0).front()->GetColliders();
	for (const auto& col : *stageColliders)
	{
		float width = 2.0f;
		if (portal)
		{
			// ignore collision on walls attached to the current colliding portal
			if (portal->GetAttachedColliderNormal() == col->GetNormal())
				continue;

			width = 0.5f;
		}

		AddPosition(Collision::ObbPolygonCollision(GetOBB(), col, width));
	}

	// check if cube landed on something
	if (startY < m_position.y)
	{
		m_velocity.y = 0;
		m_isGrounded = true;
	}
	else
	{
		m_isGrounded = false;
	}
}

void Cube::PortalFunneling()
{
	if (m_isGrounded)
		return;

	// check if both portals normals are up or down
	dx::XMFLOAT3 normal;
	float maxDiff = 0.6f;

	if (auto entrance = PortalManager::GetPortal(m_entrancePortal))
	{
		normal = entrance->GetForward();
		if (fabsf(normal.x) < 0.01f && fabsf(normal.z) < 0.01f)
		{
			if (auto exit = PortalManager::GetPortal(m_entrancePortal)->GetLinkedPortal())
			{
				normal = exit->GetForward();
				if (fabsf(normal.x) < 0.01f && fabsf(normal.z) < 0.01f)
				{
					// move the cube closer to portal if both portals are close on the xz plane
					dx::XMFLOAT3 pos1 = entrance->GetPositionFloat();
					dx::XMFLOAT3 pos2 = exit->GetPositionFloat();
					if (fabsf(pos1.x - pos2.x) <= maxDiff && fabsf(pos1.z - pos2.z) <= maxDiff)
					{
						dx::XMFLOAT3 lerpPos = entrance->GetPositionFloat();
						lerpPos.y = GetPositionFloat().y;

						SetPosition(Lerp(GetPositionFloat(), lerpPos, 0.1f));
						m_velocity.x = 0;
						m_velocity.z = 0;
					}
				}
			}
		}
	}
}

void Cube::Draw(Pass pass)
{
	if (!(pass == Pass::Default || pass == Pass::PortalBlue || pass == Pass::PortalOrange))
		return;

	GameObject::Draw(pass);
	m_obb.Draw();

	// set buffers
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	MATERIAL mat = {};
	mat.Diffuse = { 1,1,1,1 };
	mat.Specular = { 1,1,1,1 };
	m_shader->SetMaterial(mat);

	if (pass == Pass::PortalBlue)
	{
		m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Blue));
		m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Blue));
	}
	else if (pass == Pass::PortalOrange)
	{
		m_shader->SetViewMatrix(&PortalManager::GetViewMatrix(PortalType::Orange));
		m_shader->SetProjectionMatrix(&PortalManager::GetProjectionMatrix(PortalType::Orange));
	}

	// set the stencil forcefully to 0 to prevent portal backface rendering over the cube
	if(pass == Pass::Default)
		CRenderer::SetDepthStencilState(4, 0);

	// draw the model
	CRenderer::DrawModel(m_shader, m_model);

	// draw the cloned model
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		if (auto linked = portal->GetLinkedPortal())
		{
			m_shader->SetPortalInverseWorldMatrix(true, &dx::XMMatrixInverse(nullptr, linked->GetWorldMatrix()));
			dx::XMMATRIX world = portal->GetClonedOrientationMatrix(GetWorldMatrix());
			m_shader->SetWorldMatrix(&world);
			CRenderer::DrawModel(m_shader, m_model);

			m_shader->SetPortalInverseWorldMatrix(false);
		}
	}

	// set the stencil back
	if (pass == Pass::Default)
		CRenderer::SetDepthStencilState(6, 0);
}

void Cube::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (!(pass == Pass::Lightmap))
		return;

	GameObject::Draw(shader, pass);

	// set shader buffers
	dx::XMMATRIX world = GetWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetViewMatrix());

	// draw the model
	CRenderer::DrawModel(shader, m_model);
}

void Cube::Swap()
{
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		// get the cloned orientation
		dx::XMVECTOR clonedScale, clonedRot, clonedPos;
		dx::XMMatrixDecompose(&clonedScale, &clonedRot, &clonedPos, portal->GetClonedOrientationMatrix(GetWorldMatrix()));

		// swap the scale, rotation and position
		SetScale(clonedScale);
		SetRotation(clonedRot);
		SetPosition(clonedPos);

		// swap the velocity
		dx::XMFLOAT3 adjustedVel = m_velocity;
		if(fabsf(portal->GetForward(true).y != 1) || fabsf(portal->GetLinkedPortal()->GetForward(true).y) != 1)
			adjustedVel.y *= 1.3f;

		dx::XMVECTOR vel = dx::XMLoadFloat3(&adjustedVel);
		dx::XMStoreFloat3(&m_velocity, portal->GetClonedVelocity(vel));

		if (portal->GetType() == PortalType::Blue)
		{
			SetEntrancePortal(PortalType::Orange);
		}
		else
		{
			SetEntrancePortal(PortalType::Blue);
		}
	}
}
