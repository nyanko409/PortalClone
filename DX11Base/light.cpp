#include "pch.h"
#include "light.h"
#include "camera.h"
#include "manager.h"
#include "main.h"


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

dx::XMMATRIX LightManager::GetProjectionMatrix()
{
	auto cam = CManager::GetActiveScene()->GetMainCamera();
	//return dx::XMMatrixOrthographicLH(15, 15, cam->GetNearClip(), cam->GetFarClip());
	return dx::XMMatrixPerspectiveFovLH(1.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 100.0f);
}

dx::XMMATRIX LightManager::GetViewMatrix()
{
	dx::XMVECTOR pos = CManager::GetActiveScene()->GetMainCamera()->GetPosition();
	dx::XMVECTOR focus = pos;
	dx::XMVECTOR up = dx::XMVectorSet(0, 0, 1, 0);
	
	pos = dx::XMVectorSetY(pos, dx::XMVectorGetY(pos) + 15);

	return dx::XMMatrixLookAtLH(pos, focus, up);
}

void LightManager::UninitLighting()
{
	SAFE_DELETE(m_dirLight);
}
