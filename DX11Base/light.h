#pragma once


struct DirectionalLight
{
	dx::XMFLOAT4	Direction;
	dx::XMFLOAT4	Diffuse;
	dx::XMFLOAT4	Ambient;
};

static class LightManager
{
public:
	static void SetDirectionalLight(dx::XMFLOAT4 direction, dx::XMFLOAT4 diffuse, dx::XMFLOAT4 ambient);
	static DirectionalLight* GetDirectionalLight() { return m_dirLight; }

	static dx::XMMATRIX GetProjectionMatrix();
	static dx::XMMATRIX GetViewMatrix();

	static void UninitLighting();

private:
	static DirectionalLight* m_dirLight;
};
