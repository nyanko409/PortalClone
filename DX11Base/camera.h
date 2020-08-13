#pragma once

#include "gameObject.h"


class Camera : public GameObject
{
protected:
	dx::XMFLOAT4X4 m_mView, m_mProjection;
	dx::XMFLOAT3 m_target;
	dx::XMFLOAT3 m_forward, m_right;

	float m_nearClip;
	float m_farClip;

	void SetViewMatrix();
	void SetprojectionMatix();

public:
	virtual void Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	dx::XMMATRIX GetViewMatrix() const { return dx::XMLoadFloat4x4(&m_mView); }
	dx::XMMATRIX GetProjectionMatrix() const { return dx::XMLoadFloat4x4(&m_mProjection); }
};
