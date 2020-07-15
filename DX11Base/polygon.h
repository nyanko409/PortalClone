#pragma once

#include "gameObject.h"


class CPolygon : public GameObject
{
private:
	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;

public:
	void Init();
	void Uninit();
	void Update();
	void Draw();
};
