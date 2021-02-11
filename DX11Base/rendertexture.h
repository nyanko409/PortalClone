#pragma once


enum class RenderTextureType
{
	Custom, Shadowmap
};

class RenderTexture
{
public:
	RenderTexture() = delete;
	RenderTexture(uint8_t renderTargetViewID, UINT width, UINT height, RenderTextureType type);
	~RenderTexture();

	ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView; }
	ID3D11ShaderResourceView* GetRenderTexture() const { return m_resourceView; }
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_DepthStencilView; }
	D3D11_VIEWPORT* GetViewPort() const { return m_viewPort; }
	uint8_t GetRenderTargetViewID() const { return m_renderTargetViewID; }

private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11ShaderResourceView* m_resourceView = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	D3D11_VIEWPORT* m_viewPort = nullptr;
	UINT m_renderTargetViewID;

	void CreateCustom(uint8_t renderTargetViewID, UINT width, UINT height);
	void CreateShadowmap(uint8_t renderTargetViewID, UINT width, UINT height);
};
