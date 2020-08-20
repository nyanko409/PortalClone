#pragma once

#include "camera.h"


class TopDownCamera : public Camera
{
public:
	void Init() override;
	void Uninit() override;
	void Draw() override;
	void Update() override;
};
