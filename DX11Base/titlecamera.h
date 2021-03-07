#pragma once

#include "camera.h"


class TitleCamera : public Camera
{
public:
	void Init() override;
	void Uninit() override;
	void Draw(Pass pass) override;
	void Update() override;

private:
	void SetViewMatrix() override;
};
