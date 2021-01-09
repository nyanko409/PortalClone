#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "obbcollider.h"


class Cube : public GameObject
{
public:
	Cube() {}
	~Cube() {}

	void Awake() override;
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<Shader>& shader, Pass pass) override;

	OBB* GetObb() { return &m_obb; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<Model> m_model;
	OBB m_obb;
};
