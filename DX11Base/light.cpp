#include "pch.h"
#include "light.h"
#include "camera.h"
#include "manager.h"


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

dx::XMMATRIX LightManager::GetDirectionalProjectionMatrix()
{
	auto cam = CManager::GetActiveScene()->GetMainCamera();
	//return dx::XMMatrixOrthographicOffCenterLH(-10, 10, -10, 10, 0.1F, 100);
	return dx::XMMatrixOrthographicLH(100, 100, cam->GetNearClip(), cam->GetFarClip());
}

dx::XMMATRIX LightManager::GetDirectionalViewMatrix()
{
	dx::XMVECTOR dir = dx::XMLoadFloat4(&LightManager::GetDirectionalLight()->Direction);
	dir = dx::XMVectorScale(dir, -100);
	dx::XMVECTOR focus = dx::XMVectorSet(0, 0, 0, 0);
	dx::XMVECTOR up = dx::XMVectorSet(0, 1, 0, 0);

	return dx::XMMatrixLookAtLH(dir, focus, up);
}

void LightManager::UninitLighting()
{
	SAFE_DELETE(m_dirLight);
}
