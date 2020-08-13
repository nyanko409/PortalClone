#pragma once

#include "gameObject.h"
#include "basiclightshader.h"


class Player : public GameObject
{
private:
	BasicLightShader* m_shader;
	std::shared_ptr<CModel> m_model;

public:
	Player() {}
	~Player() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};
