#include "pch.h"
#include "manager.h"
#include "modelmanager.h"
#include "renderer.h"
#include "model.h"
#include "player.h"
#include "cube.h"
#include "math.h"
#include "input.h"
#include "main.h"
#include "fpscamera.h"
#include "frustumculling.h"
#include "light.h"
#include "rendertexture.h"


void Cube::Awake()
{
	GameObject::Awake();

	// init player stuff
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_CUBE, m_model);

	m_position = dx::XMFLOAT3(0.0F, 1.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0f, 0, 0.0f);
	m_scale = dx::XMFLOAT3(0.2F, 0.2F, 0.2F);

	m_enableFrustumCulling = false;

	m_obb.Init((GameObject*)this, 10, 10, 10, 0, 0, -5);
}

void Cube::Init()
{
	GameObject::Init();
}

void Cube::Uninit()
{
	GameObject::Uninit();
}

void Cube::Update()
{
	GameObject::Update();
	m_obb.Update();

	m_rotation.y += 0.03F;
	//m_rotation.z += 0.4F;
	//m_rotation.x += 0.1F;
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
}

void Cube::Draw(const std::shared_ptr<Shader>& shader, Pass pass)
{
	if (pass == Pass::StencilOnly)
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
