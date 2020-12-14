#pragma once

#include "camera.h"


class FPSCamera : public Camera
{
private:
	float m_moveSpeed;
	POINT m_cursorPos, m_cursorFixedPos;
	dx::XMFLOAT3 m_forward, m_right;
	bool m_inDebugMode;
	std::weak_ptr<const GameObject> m_target;

	void SetViewMatrix() override;

	void MouseLook();
	void Movement();
	void ToggleDebugMode();

public:
	void Init() override;
	void Uninit() override;
	void Draw(Pass pass) override;
	void Update() override;

	void SetFollowTarget(const std::shared_ptr<const GameObject>& target) { m_target = target; }
	dx::XMVECTOR GetRightVector() const { return dx::XMLoadFloat3(&m_right); }
	dx::XMVECTOR GetForwardVector() const { return dx::XMLoadFloat3(&m_forward); }
	dx::XMMATRIX GetLocalToWorldMatrix();
	dx::XMVECTOR GetEyePosition() 
	{
		dx::XMVECTOR eye = dx::XMLoadFloat3(&m_position);
		eye = dx::XMVectorAdd(eye, dx::XMVectorScale(GetForwardVector(), 1));
		eye = dx::XMVectorAdd(eye, { 0,3,0 });

		return eye;
	}
};
