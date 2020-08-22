#pragma once

#include "gameObject.h"
#include "basiclightshader.h"


class BulletSpawner : public GameObject
{
public:
	BulletSpawner() {}
	~BulletSpawner() {}

	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

private:
	BasicLightShader* m_shader;
	std::shared_ptr<CModel> m_model;
};
