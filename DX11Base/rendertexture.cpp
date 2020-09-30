#include "pch.h"
#include "rendertexture.h"
#include "main.h"
#include "renderer.h"


RenderTexture::RenderTexture()
{
	auto pDevice = CRenderer::GetDevice();

	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
  	memset( &texDesc, 0, sizeof( texDesc ) );
  	texDesc.Usage              = D3D11_USAGE_DEFAULT;
  	texDesc.Format             = DXGI_FORMAT_R8G8B8A8_TYPELESS;
  	texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  	texDesc.Width              = SCREEN_WIDTH;
  	texDesc.Height             = SCREEN_HEIGHT;
  	texDesc.CPUAccessFlags     = 0;
  	texDesc.MipLevels          = 1;
  	texDesc.ArraySize          = 1;
  	texDesc.SampleDesc.Count   = 1;
  	texDesc.SampleDesc.Quality = 0;
  
  	// 2次元テクスチャの生成
  	HRESULT hr = pDevice->CreateTexture2D( &texDesc, NULL, &m_texture );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create render texture!", "Error!", MB_OK);
		return;
  	}
  
  	// レンダーターゲットビューの設定
  	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
  	memset( &rtvDesc, 0, sizeof( rtvDesc ) );
  	rtvDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
  	rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
  
  	// レンダーターゲットビューの生成
  	hr = pDevice->CreateRenderTargetView(m_texture, &rtvDesc, &m_renderTargetView );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create render target view!", "Error!", MB_OK);
		return;
  	}
  
  	// シェーダリソースビューの設定
  	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  	memset( &srvDesc, 0, sizeof( srvDesc ) );
  	srvDesc.Format              = rtvDesc.Format;
  	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
  	srvDesc.Texture2D.MipLevels = 1;
  	
  	// シェーダリソースビューの生成
  	hr = pDevice->CreateShaderResourceView( m_texture, &srvDesc, &m_resourceView );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create shader resource view!", "Error!", MB_OK);
		return;
  	}
  
  	// サンプラステートの設定
  	D3D11_SAMPLER_DESC smpDesc;
  	memset( &smpDesc, 0, sizeof( smpDesc ) );
  	smpDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  	smpDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
  	smpDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
  	smpDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
  	smpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  	smpDesc.MinLOD         = 0;
  	smpDesc.MaxLOD         = D3D11_FLOAT32_MAX;
  
  	// サンプラステート生成
  	hr = pDevice->CreateSamplerState( &smpDesc, &m_samplerState );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create the sampler state!", "Error!", MB_OK);
		return;
  	}
}

RenderTexture::~RenderTexture()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_resourceView);
	SAFE_RELEASE(m_samplerState);
}
