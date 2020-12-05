#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "field.h"
#include "modelmanager.h"
#include "manager.h"
#include "bullet.h"
#include "light.h"
#include "rendertexture.h"


void Field::Init()
{
	GameObject::Init();

	// get the shader
	m_shader = CRenderer::GetShader<BasicLightShader>();

	ModelManager::GetModel(MODEL_FLOOR, m_model);

	// init values
	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(1.0F, 1.0F, 1.0F);

	m_enableFrustumCulling = false;

	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, 50, 20, 0, 0, -1, 0, 10, 39.9F);
	m_colliders.push_back(new PolygonCollider());
	m_colliders.back()->Init(this, 50, 20, 0, 0, 1, 0, 10, 0.9F);
}

void Field::Uninit()
{
	GameObject::Uninit();

	for (auto& collider : m_colliders)
		delete collider;

	m_colliders.clear();
}

void Field::Update()
{
	GameObject::Update();

	for (auto collider : m_colliders)
		collider->Update();
}

void Field::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	if (renderPass == 1)
	{
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

		// draw the model
		CRenderer::DrawModel(m_shader, m_model);
		for (auto collider : m_colliders)
			collider->Draw();
	}
}
