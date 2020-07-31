#pragma once

#include "camera.h"


class FPSCamera : public Camera
{
private:
	float m_moveSpeed;
	POINT m_cursorPos, m_cursorFixedPos;
	bool m_inDebugMode;

	void MouseLook();
	void Movement();
	void Shoot();
	void ToggleDebugMode();

public:
	void Init() override;
	void Uninit() override;
	void Draw() override;
	void Update() override;
};
