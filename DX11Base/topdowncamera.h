#pragma once

#include "camera.h"


class TopDownCamera : public Camera
{
public:
	void Init() override;
	void Uninit() override;
	void Draw() override;
	void Update() override;

	void SetViewMatrix() override;

	void SetTarget(GameObject* target) { m_target = target; }

private:
	dx::XMFLOAT3 m_offset;
	GameObject* m_target;
};
