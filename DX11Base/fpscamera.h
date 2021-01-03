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
	bool InDebugMode() { return m_inDebugMode; }
	dx::XMVECTOR GetRightVector() const { return dx::XMLoadFloat3(&m_right); }
	dx::XMVECTOR GetForwardVector() const { return dx::XMLoadFloat3(&m_forward); }
	dx::XMMATRIX GetLocalToWorldMatrix();
	dx::XMVECTOR GetEyePosition() 
	{
		dx::XMVECTOR eye = dx::XMLoadFloat3(&m_position);
		eye = dx::XMVectorAdd(eye, { 0,3,0 });

		return eye;
	}

	void SwapCamera(dx::XMFLOAT3 forward) 
	{ 
		// update the forward vector
		m_forward = forward; 

		// calculate right from new forward
		dx::XMMATRIX nRot = dx::XMMatrixRotationY(dx::XMConvertToRadians(90));

		forward.y = 0;
		dx::XMVECTOR vecForward = dx::XMLoadFloat3(&forward);

		dx::XMVECTOR right = dx::XMVector3Transform(vecForward, nRot);
		dx::XMStoreFloat3(&m_right, right);

		// update the position if following a target
		if (auto target = m_target.lock())
			dx::XMStoreFloat3(&m_position, target->GetPosition());
	}
};
