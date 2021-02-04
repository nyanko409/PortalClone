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
	m_scale = dx::XMFLOAT3(0.2F, 0.2F, 0.2F);

	m_entrancePortal = PortalType::None;
	lastTravel = TravelType::None;
	m_enableFrustumCulling = false;
	m_velocity = { 0,0,0 };

	m_obb.Init((GameObject*)this, 10, 10, 10, 0, 0, 0);
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

	// apply gravity
	m_velocity.y -= 0.05f;

	// clamp y velocity
	if (m_velocity.y < -1.2f)
		m_velocity.y = -1.2f;

	// update position
	AddPosition(m_velocity);

	// collision
	UpdateCollision();
}

void Cube::UpdateCollision()
{
	m_obb.Update();
	dx::XMFLOAT3 intersection = { 0,0,0 };

	// portal collision
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		auto colliders = portal->GetEdgeColliders();
		for (auto col : *colliders)
		{
			intersection += Collision::ObbObbCollision(&m_obb, col);
		}
	}

	// stage collision
	auto stageColliders = CManager::GetActiveScene()->GetGameObjects<Stage>(0).front()->GetColliders();
	for (const auto& col : *stageColliders)
	{
		// ignore collision on walls attached to the current colliding portal
		if (auto portal = PortalManager::GetPortal(GetEntrancePortal()))
		{
			if (portal->GetAttachedColliderId() == col->GetId())
				continue;
		}

		intersection += Collision::ObbPolygonCollision(GetOBB(), col);
	}

	AddPosition(intersection);

	// check if cube landed on something
	if (intersection.y > 0.0f)
	{
		m_velocity.y = 0;
	}
}

void Cube::Draw(Pass pass)
{
	if (!(pass == Pass::Default || pass == Pass::PortalBlue || pass == Pass::PortalOrange))
		return;

	GameObject::Draw(pass);
	m_obb.Draw();

	// set buffers
	m_shader->SetDirectionalLight(LightManager::GetDirectionalLight());

	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	MATERIAL mat = {};
	mat.Diffuse = { 1,1,1,1 };
	mat.Specular = { 1,1,1,1 };
	m_shader->SetMaterial(mat);

	m_shader->SetShadowMapTexture(CRenderer::GetRenderTexture(2)->GetRenderTexture());
	m_shader->SetLightProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	m_shader->SetLightViewMatrix(&LightManager::GetDirectionalViewMatrix());

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

	// draw the model
	CRenderer::DrawModel(m_shader, m_model);

	// draw the cloned model
	if (auto portal = PortalManager::GetPortal(m_entrancePortal))
	{
		dx::XMMATRIX world = portal->GetClonedOrientationMatrix(GetWorldMatrix());
		m_shader->SetWorldMatrix(&world);
		CRenderer::DrawModel(m_shader, m_model);
	}
}

void Cube::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (!(pass == Pass::Lightmap))
		return;

	GameObject::Draw(shader, pass);

	// set shader buffers
	dx::XMMATRIX world = GetWorldMatrix();
	shader->SetWorldMatrix(&world);

	shader->SetProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	shader->SetViewMatrix(&LightManager::GetDirectionalViewMatrix());

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
		//dx::XMVECTOR vel = dx::XMLoadFloat3(&m_velocity);
		//dx::XMStoreFloat3(&m_velocity, portal->GetClonedVelocity(vel));

		if (portal->GetType() == PortalType::Blue)
		{
			lastTravel = TravelType::BlueOrange;
			SetEntrancePortal(PortalType::Orange);
		}
		else
		{
			lastTravel = TravelType::OrangeBlue;
			SetEntrancePortal(PortalType::Blue);
		}
	}
}
