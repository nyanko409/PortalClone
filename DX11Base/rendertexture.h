#pragma once


class RenderTexture
{
public:
	RenderTexture() = delete;
	RenderTexture(uint8_t renderTargetViewID, UINT width, UINT height, bool createDepthStencilView);
	~RenderTexture();

	ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView; }
	ID3D11ShaderResourceView* GetRenderTexture() { return m_resourceView; }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; }
	D3D11_VIEWPORT* GetViewPort() { return m_viewPort; }
	uint8_t GetRenderTargetViewID() { return m_renderTargetViewID; }

private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11ShaderResourceView* m_resourceView = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	D3D11_VIEWPORT* m_viewPort = nullptr;
	UINT m_renderTargetViewID;
};
