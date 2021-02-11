#include "pch.h"
#include "rendertexture.h"
#include "main.h"
#include "renderer.h"


RenderTexture::RenderTexture(uint8_t renderTargetViewID, UINT width, UINT height, RenderTextureType type)
{
	if (type == RenderTextureType::Custom)
		CreateCustom(renderTargetViewID, width, height);
	else if (type == RenderTextureType::Shadowmap)
		CreateShadowmap(renderTargetViewID, width, height);
}

RenderTexture::~RenderTexture()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_resourceView);
	SAFE_RELEASE(m_DepthStencilView);
	SAFE_DELETE(m_viewPort);
}

void RenderTexture::CreateCustom(uint8_t renderTargetViewID, UINT width, UINT height)
{
	auto pDevice = CRenderer::GetDevice();
	m_renderTargetViewID = renderTargetViewID;

	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	//texDesc.Format				 = DXGI_FORMAT_R8G8B8A8_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.CPUAccessFlags = 0;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	HRESULT hr = pDevice->CreateTexture2D(&texDesc, NULL, &m_texture);
	if (FAILED(hr))
	{
		MessageBox(GetWindow(), "Failed to create render texture!", "Error!", MB_OK);
		exit(-1);
	}

	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	//rtvDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(m_texture, &rtvDesc, &m_renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(GetWindow(), "Failed to create render target view!", "Error!", MB_OK);
		exit(-1);
	}

	// シェーダリソースビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = rtvDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(m_texture, &srvDesc, &m_resourceView);
	if (FAILED(hr))
	{
		MessageBox(GetWindow(), "Failed to create shader resource view!", "Error!", MB_OK);
		exit(-1);
	}
}

void RenderTexture::CreateShadowmap(uint8_t renderTargetViewID, UINT width, UINT height)
{
	auto pDevice = CRenderer::GetDevice();
	m_renderTargetViewID = renderTargetViewID;

	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32_TYPELESS;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	pDevice->CreateTexture2D(&td, NULL, &m_texture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	pDevice->CreateDepthStencilView(m_texture, &dsvd, &m_DepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	pDevice->CreateShaderResourceView(m_texture, &SRVDesc, &m_resourceView);
}
