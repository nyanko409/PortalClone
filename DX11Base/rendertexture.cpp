#include "pch.h"
#include "rendertexture.h"
#include "main.h"
#include "renderer.h"


RenderTexture::RenderTexture(uint8_t renderTargetViewID, UINT width, UINT height, bool createDepthStencilView)
{
	auto pDevice = CRenderer::GetDevice();
	m_renderTargetViewID = renderTargetViewID;

	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
  	memset( &texDesc, 0, sizeof( texDesc ) );
  	texDesc.Usage				 = D3D11_USAGE_DEFAULT;
	texDesc.Format				 = DXGI_FORMAT_R16G16B16A16_FLOAT;
  	texDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  	texDesc.Width				 = width;
  	texDesc.Height				 = height;
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

	// return if depth stencil view is not needed
	if (!createDepthStencilView)
		return;

	// ビューポート設定
	m_viewPort = new D3D11_VIEWPORT;
	m_viewPort->Width = (FLOAT)width;
	m_viewPort->Height = (FLOAT)height;
	m_viewPort->MinDepth = 0.0f;
	m_viewPort->MaxDepth = 1.0f;
	m_viewPort->TopLeftX = 0;
	m_viewPort->TopLeftY = 0;

	// create the depth stencil view
	ID3D11Texture2D* depthTexture = NULL;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc = texDesc.SampleDesc;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	pDevice->CreateTexture2D(&td, NULL, &depthTexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags = 0;
	pDevice->CreateDepthStencilView(depthTexture, &dsvd, &m_DepthStencilView);
	delete depthTexture;
}

RenderTexture::~RenderTexture()
{
	SAFE_RELEASE(m_texture);
	SAFE_RELEASE(m_renderTargetView);
	SAFE_RELEASE(m_resourceView);
	SAFE_RELEASE(m_DepthStencilView);
	SAFE_DELETE(m_viewPort);
}
