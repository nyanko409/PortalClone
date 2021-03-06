#pragma once

#include "gameObject.h"
#include "basiclightshader.h"
#include "collision.h"
#include "portaltraveler.h"
#include "grabbable.h"


class Cube : public GameObject, public PortalTraveler, public Grabbable
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

	void Swap() override;
	dx::XMVECTOR GetTravelerPosition() const override { return GetPosition(); }

private:
	std::shared_ptr<BasicLightShader> m_shader;
	std::shared_ptr<Model> m_model;
	bool m_isGrounded;

	void UpdateCollision();
	void PortalFunneling();
};
