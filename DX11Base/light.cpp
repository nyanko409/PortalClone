#include "pch.h"
#include "renderer.h"
#include "light.h"


DirectionalLight* LightManager::m_dirLight = nullptr;


void LightManager::SetDirectionalLight(dx::XMFLOAT4 direction, dx::XMFLOAT4 diffuse, dx::XMFLOAT4 ambient)
{
	if (!m_dirLight)
		m_dirLight = new DirectionalLight();

	// normalize the light direction
	dx::XMVECTOR dir = dx::XMLoadFloat4(&direction);
	dir = dx::XMVector3Normalize(dir);
	dx::XMStoreFloat4(&direction, dir);

	// set the light
	m_dirLight->Direction = direction;
	m_dirLight->Diffuse = diffuse;
	m_dirLight->Ambient = ambient;
}

void LightManager::UninitLighting()
{
	SAFE_DELETE(m_dirLight);
}
