#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "polygoncollider.h"


class Stage : public GameObject
{
public:
	Stage() {}
	~Stage() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(Pass pass) override;
	void Draw(const std::shared_ptr<Shader>& shader, Pass pass) override;

	const std::vector<PolygonCollider*>* GetColliders() const { return &m_colliders; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;
	std::vector<PolygonCollider*> m_colliders;
};
