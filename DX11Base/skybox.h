#pragma once

#include "gameObject.h"


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
	std::shared_ptr<CModel> m_model;
	GameObject* center;
};
