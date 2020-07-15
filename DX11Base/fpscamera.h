#pragma once

#include "camera.h"


class FPSCamera : public Camera
{
private:
	float m_moveSpeed;
	POINT m_cursorPos, m_cursorFixedPos;

	void MouseLook();
	void Movement();
	void Shoot();

public:
	void Init() override;
	void Uninit() override;
	void Draw() override;
	void Update() override;
};
