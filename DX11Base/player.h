#pragma once

#include "gameObject.h"


class Player : public GameObject
{
private:
	std::shared_ptr<CModel> m_model;

public:
	Player() {}
	~Player() {}

	void Init();
	void Uninit();
	void Update();
	void Draw();
};
