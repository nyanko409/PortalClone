#pragma once

#include "gameObject.h"


class Camera : public GameObject
{
protected:
	D3DXMATRIX m_mView, m_mProjection;
	bool m_perspective;
	D3DXVECTOR3 m_target;
	D3DXVECTOR3 m_forward, m_right;

	float m_nearClip = 0.1F;
	float m_farClip = 100.0F;

	void SetViewMatrix();
	void SetprojectionMatix();
	void Set2DProjection();

public:
	virtual void Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	void SetPerspective(const bool value) { m_perspective = value; }
	D3DXMATRIX GetViewMatrix() const { return m_mView; }
	D3DXMATRIX GetProjectionMatrix() const { return m_mProjection; }
};
