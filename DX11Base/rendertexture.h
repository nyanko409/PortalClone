#pragma once


class RenderTexture
{
public:
	RenderTexture();
	~RenderTexture();

	ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView; }
	ID3D11ShaderResourceView* GetRenderTexture() { return m_resourceView; }
	ID3D11SamplerState* GetSamplerState() { return m_samplerState; }

private:
	ID3D11Texture2D* m_texture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_resourceView;
	ID3D11SamplerState* m_samplerState;
};
