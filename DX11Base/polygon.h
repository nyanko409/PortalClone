#pragma once

#include "gameObject.h"
#include "uishader.h"


class CPolygon : public GameObject
{
protected:
	Shader* m_shader;
	ID3D11Buffer*				m_VertexBuffer = nullptr;
	ID3D11ShaderResourceView*	m_Texture = nullptr;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void CreatePlane(float centerX, float centerY, float width, float height);
	void LoadTexture(const char* filename);
};
