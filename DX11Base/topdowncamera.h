#pragma once

#include "camera.h"


class TopDownCamera : public Camera
{
public:
	void Init() override;
	void Uninit() override;
	void Draw(Pass pass) override;
	void Update() override;

	void SetViewMatrix() override;

private:
	dx::XMFLOAT3 m_offset;
	std::weak_ptr<GameObject> m_target;
	float m_lerpSpeed;
};
