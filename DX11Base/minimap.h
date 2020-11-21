#pragma once

#include "gameobject.h"
#include "minimapshader.h"
#include "rendertexture.h"


class Minimap : public GameObject
{
public:
	void Awake() override;
	void Uninit() override;
	void Update() override;
	void Draw(UINT renderPass) override;

	void SetTexture(const char* path);

private:
	std::shared_ptr<MinimapShader> m_shader;
	std::unique_ptr<RenderTexture> m_renderTexture;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11ShaderResourceView* m_Texture;
};
