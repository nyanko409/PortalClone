#include "pch.h"
#include "renderer.h"
#include "modelmanager.h"
#include "player.h"
#include "portal.h"
#include "modelmanager.h"
#include "manager.h"


void Portal::Awake()
{
	GameObject::Awake();

	// get the shader
	m_shader = CRenderer::GetShader<PortalShader>();

	ModelManager::GetModel(MODEL_PORTAL, m_model);

	// init values
	m_position = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_rotation = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	m_scale = dx::XMFLOAT3(5.0F, 5.0F, 5.0F);

	m_enableFrustumCulling = false;
}

void Portal::Uninit()
{
	GameObject::Uninit();
}

void Portal::Update()
{
	GameObject::Update();
}

void Portal::Draw(UINT renderPass)
{
	GameObject::Draw(renderPass);

	if (renderPass == 1)
	{
		// set buffers
		dx::XMMATRIX world = GetWorldMatrix();
		m_shader->SetWorldMatrix(&world);

		// draw the model
		CRenderer::DrawModel(m_shader, m_model);
	}
}
