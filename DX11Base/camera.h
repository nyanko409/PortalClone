#pragma once

#include "gameObject.h"


class Camera : public GameObject
{
public:
	virtual void Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw(Pass pass) override;

	dx::XMMATRIX GetViewMatrix() const { return dx::XMLoadFloat4x4(&m_mView); }
	dx::XMMATRIX GetProjectionMatrix() const { return dx::XMLoadFloat4x4(&m_mProjection); }
	dx::XMMATRIX CalculateObliqueMatrix(dx::XMVECTOR clipPlane);

	float GetNearClip() { return m_nearClip; }
	float GetFarClip() { return m_farClip; }

protected:
	dx::XMFLOAT4X4 m_mView, m_mProjection;

	float m_nearClip;
	float m_farClip;

	virtual void SetViewMatrix() = 0;
	virtual void SetProjectionMatix();
	
	void SetCameraPositionBuffers();
};
