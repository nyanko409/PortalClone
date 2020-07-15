#pragma once

#include "gameObject.h"


class Billboard : public GameObject
{
private:
	float width, height;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11ShaderResourceView* m_texture;
	int m_count;

public:
	void Init() override;
	void Uninit() override;
	void Draw() override;
	void Update() override;
};
