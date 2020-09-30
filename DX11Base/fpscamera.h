#pragma once

#include "camera.h"


class FPSCamera : public Camera
{
private:
	float m_moveSpeed;
	POINT m_cursorPos, m_cursorFixedPos;
	dx::XMFLOAT3 m_forward, m_right;
	bool m_inDebugMode;

	void SetViewMatrix() override;

	void MouseLook();
	void Movement();
	void Shoot();
	void ToggleDebugMode();

public:
	void Init() override;
	void Uninit() override;
	void Draw(UINT renderPass) override;
	void Update() override;
};
