#pragma once

#include "gameObject.h"
#include "rangeshader.h"
#include "basiclightshader.h"
#include "polygoncollider.h"


class Field : public GameObject
{
public:
	Field() {}
	~Field() {}

	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	const std::vector<PolygonCollider*>* GetColliders() const { return &m_colliders; }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<class Model> m_model;
	std::vector<PolygonCollider*> m_colliders;
};
