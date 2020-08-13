#pragma once

#include "gameObject.h"
#include "basiclightshader.h"


class Skybox : public GameObject
{
public:
	Skybox() {}
	~Skybox() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

private:
	BasicLightShader* m_shader;
	std::shared_ptr<CModel> m_model;
	GameObject* m_center;
};
