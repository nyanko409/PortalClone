#include "pch.h"
#include "rendertexture.h"
#include "main.h"
#include "renderer.h"


RenderTexture::RenderTexture(uint8_t renderTargetViewID)
{
	auto pDevice = CRenderer::GetDevice();
	m_renderTargetViewID = renderTargetViewID;

	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
  	memset( &texDesc, 0, sizeof( texDesc ) );
  	texDesc.Usage				 = D3D11_USAGE_DEFAULT;
	texDesc.Format				 = DXGI_FORMAT_R32G32B32A32_FLOAT;
  	texDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  	texDesc.Width				 = SCREEN_WIDTH * 2;
  	texDesc.Height				 = SCREEN_HEIGHT * 2;
  	texDesc.CPUAccessFlags		 = 0;
  	texDesc.MipLevels			 = 1;
  	texDesc.ArraySize			 = 1;
  	texDesc.SampleDesc.Count	 = 1;
  	texDesc.SampleDesc.Quality	 = 0;
  
  	HRESULT hr = pDevice->CreateTexture2D( &texDesc, NULL, &m_texture );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create render texture!", "Error!", MB_OK);
		exit(-1);
  	}
  
  	// レンダーターゲットビューの設定
  	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
  	memset( &rtvDesc, 0, sizeof( rtvDesc ) );
	rtvDesc.Format				= texDesc.Format;
  	rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice	= 0;
  
  	hr = pDevice->CreateRenderTargetView(m_texture, &rtvDesc, &m_renderTargetView );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create render target view!", "Error!", MB_OK);
		exit(-1);
  	}
  
  	// シェーダリソースビューの設定
  	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  	memset( &srvDesc, 0, sizeof( srvDesc ) );
  	srvDesc.Format              = rtvDesc.Format;
  	srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
  	srvDesc.Texture2D.MipLevels = 1;
  	
  	hr = pDevice->CreateShaderResourceView( m_texture, &srvDesc, &m_resourceView );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create shader resource view!", "Error!", MB_OK);
		exit(-1);
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
  
  	hr = pDevice->CreateSamplerState( &smpDesc, &m_samplerState );
  	if ( FAILED( hr ) )
  	{
		MessageBox(GetWindow(), "Failed to create the sampler state!", "Error!", MB_OK);
		exit(-1);
  	}
}

RenderTexture::~RenderTexture()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_resourceView);
	SAFE_RELEASE(m_samplerState);
}
