#pragma once

#include "gameObject.h"
#include "rangeshader.h"
#include "basiclightshader.h"


class Field : public GameObject
{
private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;

public:
	Field() {}
	~Field() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	// if collided with x axis, the first bit is set (0b0001)
	// if collided with z axis, the second bit is set (0b0010)
	int CheckBounds(dx::XMFLOAT3 position, float objectRadius);
};
