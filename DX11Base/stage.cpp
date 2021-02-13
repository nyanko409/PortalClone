#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "stage.h"
#include "modelmanager.h"
#include "manager.h"
#include "light.h"
#include "rendertexture.h"


void Stage::Init()
{
	GameObject::Init();

	// get the shader
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_STAGE, m_model);

	// init values
	SetPosition(0.0F, 0.0F, 0.0F);
	SetRotation(0.0F, 0.0F, 0.0F);
	SetScale(1.0F, 1.0F, 1.0F);

	m_enableFrustumCulling = false;

	// colliders for portal
	float offset = 39.95f;
	float ceiling = 19.95f;
	float floor = 0.1f;
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, {40, 20, offset }, {-40, 20, offset }, {-40, 0, offset }, {40, 0, offset }, Wall);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { -40, 20, -offset }, { 40, 20, -offset }, { 40, 0, -offset }, { -40, 0, -offset }, Wall);
	
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { offset, 20, -40 }, { offset, 20, 40 }, { offset, 0, 40 }, { offset, 0, -40 }, Wall);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { -offset, 20, 40 }, { -offset, 20, -40 }, { -offset, 0, -40 }, { -offset, 0, 40 }, Wall);

	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { -40, ceiling, 40 }, { 40, ceiling, 40 }, { 40, ceiling, -40 }, { -40, ceiling, -40 }, Ceiling);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { 40, floor, 40 }, { -40, floor, 40 }, { -40, floor, -40 }, { 40, floor, -40 }, Floor);
}

void Stage::Uninit()
{
	GameObject::Uninit();

	for (auto& collider : m_colliders)
		delete collider;

	m_colliders.clear();
}

void Stage::Update()
{
	GameObject::Update();

	for (auto collider : m_colliders)
		collider->Update();
}

void Stage::Draw(Pass pass)
{
	if (!(pass == Pass::Default || pass == Pass::PortalBlue || pass == Pass::PortalOrange))
		return;

	GameObject::Draw(pass);

	// set buffers
	dx::XMMATRIX world = GetWorldMatrix();
	m_shader->SetWorldMatrix(&world);

	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = dx::XMFLOAT4(1.0F, 1.0F, 1.0F, 1.0F);
	m_shader->SetMaterial(material);

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

	// draw the collider
	for (auto collider : m_colliders)
		collider->Draw();
}

void Stage::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
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
