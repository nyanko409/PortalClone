#pragma once

#include "camera.h"


class TopDownCamera : public Camera
{
public:
	void Init() override;
	void Uninit() override;
	void Draw(UINT renderPass) override;
	void Update() override;

	void SetViewMatrix() override;

	bool InFrustum(dx::XMFLOAT3 position);

private:
	dx::XMFLOAT3 m_offset;
	std::weak_ptr<GameObject> m_target;
	float m_lerpSpeed;
};
