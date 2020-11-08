#pragma once

#include "polygon.h"
#include "gameobject.h"
#include "minimapshader.h"


class RenderTexture;

class Minimap : public GameObject
{
public:
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	void SetTexture(const char* path);

private:
	std::shared_ptr<MinimapShader>	m_shader;
	ID3D11Buffer*				m_VertexBuffer;
	ID3D11ShaderResourceView*	m_Texture;
	RenderTexture*				m_renderTexture;
};
