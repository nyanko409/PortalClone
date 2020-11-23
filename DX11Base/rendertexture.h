#pragma once


class RenderTexture
{
public:
	RenderTexture() = delete;
	RenderTexture(uint8_t renderTargetViewID);
	~RenderTexture();

	ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView; }
	ID3D11ShaderResourceView* GetRenderTexture() { return m_resourceView; }
	ID3D11SamplerState* GetSamplerState() { return m_samplerState; }
	uint8_t GetRenderTargetViewID() { return m_renderTargetViewID; }

private:
	ID3D11Texture2D* m_texture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_resourceView;
	ID3D11SamplerState* m_samplerState;
	UINT m_renderTargetViewID;
};
