#pragma once

#include "gameObject.h"


class Field : public GameObject
{
private:
	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;

public:
	Field() {}
	~Field() {}

	void Init();
	void Uninit();
	void Update();
	void Draw();
};
