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
	void Draw(Pass pass) override;

	void SetTexture(const char* path);
	std::shared_ptr<RenderTexture> GetRenderTexture() { return m_renderTexture; }

private:
	std::shared_ptr<MinimapShader> m_shader;
	std::shared_ptr<RenderTexture> m_renderTexture;
	ID3D11Buffer* m_VertexBuffer;
	ID3D11ShaderResourceView* m_Texture;
};
