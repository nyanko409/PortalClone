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
}

void Field::Uninit()
{
	GameObject::Uninit();
}

void Field::Update()
{
	GameObject::Update();
}

void Field::Draw(UINT renderPass)
{
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
	}
}

int Field::CheckBounds(dx::XMFLOAT3 position, float objectRadius)
{
	int result = 0;

	if (position.x + objectRadius > m_scale.x || position.x - objectRadius < -m_scale.x)
		result |= 0b0001;
	if (position.z + objectRadius > m_scale.z || position.z - objectRadius < -m_scale.z)
		result |= 0b0010;

	return result;
}
