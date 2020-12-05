#pragma once

#include "gameObject.h"
#include "rangeshader.h"
#include "portalshader.h"


class Portal : public GameObject
{
private:
	std::shared_ptr<PortalShader> m_shader;
	std::shared_ptr<class Model> m_model;

public:
	Portal() {}
	~Portal() {}

	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;
};
