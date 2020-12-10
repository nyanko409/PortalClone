#pragma once

#include "camera.h"


class TitleCamera : public Camera
{
private:
	std::weak_ptr<const GameObject> m_focus;

	void SetViewMatrix() override;

public:
	void Init() override;
	void Uninit() override;
	void Draw(UINT renderPass) override;
	void Update() override;

	void SetFocusTarget(const std::shared_ptr<const GameObject>& target) { m_focus = target; }
};
