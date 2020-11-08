#pragma once

#include "gameobject.h"
#include "uishader.h"


class Sprite : public GameObject
{
public:
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	void SetTexture(const char* path);
	void CreatePlaneCenter(float centerX, float centerY, float width, float height, bool dynamic);
	void CreatePlaneTopLeft(float topLeftX, float topLeftY, float width, float height, bool dynamic);
	void SetPositionCenter(float centerX, float centerY, float width, float height);
	void SetPositionTopLeft(float topLeftX, float topLeftY, float width, float height);

private:
	std::shared_ptr<UIShader>	m_shader;
	ID3D11Buffer*				m_VertexBuffer;
	ID3D11ShaderResourceView*	m_Texture;
};

