#pragma once

#include "gameObject.h"
#include "rangeshader.h"
#include "portalshader.h"


class Portal : public GameObject
{
private:
	std::shared_ptr<PortalShader> m_shader;
	std::shared_ptr<class Model> m_model;
	dx::XMFLOAT3 m_lookAt;
	dx::XMFLOAT3 m_up;

public:
	Portal() {}
	~Portal() {}

	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	void SetLookAt(dx::XMFLOAT3 lookAt) { m_lookAt = lookAt; }
	void SetUp(dx::XMFLOAT3 up) { m_up = up; }
};
