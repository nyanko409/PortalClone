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
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, {-40, 20, 40 }, {40, 20, 40}, {40, 0, 40}, {-40, 0, 40}, 0, 0, -1, Wall);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { -40, 20, -40 }, { 40, 20, -40 }, { 40, 0, -40 }, { -40, 0, -40 }, 0, 0, 1, Wall);
	
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { 40, 20, 40 }, { 40, 20, -40 }, { 40, 0, -40 }, { 40, 0, 40 }, -1, 0, 0, Wall);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { -40, 20, 40 }, { -40, 20, -40 }, { -40, 0, -40 }, { -40, 0, 40 }, 1, 0, 0, Wall);

	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { 40, 20, 40 }, { -40, 20, 40 }, { -40, 20, -40 }, { 40, 20, -40 }, 0, -1, 0, Ceiling);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, { 40, 0, 40 }, { -40, 0, 40 }, { -40, 0, -40 }, { 40, 0, -40 }, 0, 1, 0, Floor);
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

	m_shader->SetShadowMapTexture(CRenderer::GetRenderTexture(2)->GetRenderTexture());
	m_shader->SetLightProjectionMatrix(&LightManager::GetDirectionalProjectionMatrix());
	m_shader->SetLightViewMatrix(&LightManager::GetDirectionalViewMatrix());

	if (pass == Pass::PortalBlue)
	{
		m_shader->SetViewMatrix(&PortalStencilManager::GetViewMatrix(PortalStencilType::Blue));
		m_shader->SetProjectionMatrix(&PortalStencilManager::GetProjectionMatrix(PortalStencilType::Blue));
	}
	else if (pass == Pass::PortalOrange)
	{
		m_shader->SetViewMatrix(&PortalStencilManager::GetViewMatrix(PortalStencilType::Orange));
		m_shader->SetProjectionMatrix(&PortalStencilManager::GetProjectionMatrix(PortalStencilType::Orange));
	}

	// draw the model
	CRenderer::DrawModel(m_shader, m_model);

	// draw the collider
	for (auto collider : m_colliders)
		collider->Draw();
}

void Stage::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (pass == Pass::StencilOnly || pass == Pass::PortalBlue || pass == Pass::PortalOrange)
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
