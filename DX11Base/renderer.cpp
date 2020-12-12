#include "pch.h"
#include <io.h>
#include <typeinfo>
#include "main.h"
#include "model.h"
#include "renderer.h"
#include "gameobject.h"
#include "shader.h"
#include "computeshader.h"
#include "rendertexture.h"

// for hlsl debugging
#ifdef _DEBUG
#define DEVICE_DEBUG D3D11_CREATE_DEVICE_DEBUG
#else
#define DEVICE_DEBUG 0
#endif


D3D_FEATURE_LEVEL       CRenderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device*           CRenderer::m_D3DDevice = nullptr;
ID3D11DeviceContext*    CRenderer::m_ImmediateContext = nullptr;
IDXGISwapChain*         CRenderer::m_SwapChain = nullptr;
ID3D11RenderTargetView* CRenderer::m_RenderTargetView = nullptr;
ID3D11DepthStencilView* CRenderer::m_DepthStencilView = nullptr;
D3D11_VIEWPORT*			CRenderer::m_viewPort = nullptr;

ID3D11DepthStencilState* CRenderer::m_DepthStateEnable = nullptr;
ID3D11DepthStencilState* CRenderer::m_DepthStateDisable = nullptr;

ID3D11RasterizerState* CRenderer::m_rasterizerCullBack = nullptr;
ID3D11RasterizerState* CRenderer::m_rasterizerCullFront = nullptr;
ID3D11RasterizerState* CRenderer::m_rasterizerWireframe = nullptr;

std::map<UINT, std::shared_ptr<RenderTexture>> CRenderer::m_renderTargetViews;

std::vector<std::shared_ptr<Shader>> CRenderer::m_shaders = std::vector<std::shared_ptr<Shader>>();
std::vector<std::shared_ptr<ComputeShader>> CRenderer::m_computeShaders = std::vector<std::shared_ptr<ComputeShader>>();

std::weak_ptr<Shader> CRenderer::m_activeShader;


void CRenderer::Init()
{
	HRESULT hr = S_OK;
	
	// �f�o�C�X�A�X���b�v�`�F�[���A�R���e�L�X�g����
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = GetWindow();
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										DEVICE_DEBUG,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&sd,
										&m_SwapChain,
										&m_D3DDevice,
										&m_FeatureLevel,
										&m_ImmediateContext );


	// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
	ID3D11Texture2D* pBackBuffer = NULL;
	m_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	m_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_RenderTargetView );
	pBackBuffer->Release();

	//�X�e���V���p�e�N�X�`���[�쐬
	ID3D11Texture2D* depthTexture = NULL;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory( &td, sizeof(td) );
	td.Width			= sd.BufferDesc.Width;
	td.Height			= sd.BufferDesc.Height;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc		= sd.SampleDesc;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags	= 0;
    td.MiscFlags		= 0;
	m_D3DDevice->CreateTexture2D( &td, NULL, &depthTexture );

	//�X�e���V���^�[�Q�b�g�쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	m_D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &m_DepthStencilView );

	// �r���[�|�[�g�ݒ�
	m_viewPort = new D3D11_VIEWPORT;
	m_viewPort->Width = (FLOAT)SCREEN_WIDTH;
	m_viewPort->Height = (FLOAT)SCREEN_HEIGHT;
	m_viewPort->MinDepth = 0.0f;
	m_viewPort->MaxDepth = 1.0f;
	m_viewPort->TopLeftX = 0;
	m_viewPort->TopLeftY = 0;

	// create backface culling rasterizer state
	D3D11_RASTERIZER_DESC rd; 
	ZeroMemory( &rd, sizeof( rd ) );
	rd.FillMode = D3D11_FILL_SOLID; 
	rd.CullMode = D3D11_CULL_BACK; 
	rd.DepthClipEnable = TRUE; 
	rd.MultisampleEnable = FALSE; 

	m_D3DDevice->CreateRasterizerState( &rd, &m_rasterizerCullBack);

	// create frontface culling rasterizer state
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;

	m_D3DDevice->CreateRasterizerState(&rd, &m_rasterizerCullFront);

	// create rasterizer with wireframe mode
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_WIREFRAME;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = TRUE;
	rd.MultisampleEnable = FALSE;

	m_D3DDevice->CreateRasterizerState(&rd, &m_rasterizerWireframe);

	// set the rasterizer state
	SetRasterizerState(RasterizerState_CullBack);

	// �u�����h�X�e�[�g�ݒ�
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	ID3D11BlendState* blendState = NULL;
	m_D3DDevice->CreateBlendState( &blendDesc, &blendState );
	m_ImmediateContext->OMSetBlendState( blendState, blendFactor, 0xffffffff );

	// �[�x�X�e���V���X�e�[�g�ݒ�
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	m_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &m_DepthStateEnable );//�[�x�L���X�e�[�g
	m_ImmediateContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );
}

void CRenderer::Uninit()
{
	for (auto shader : m_shaders)
		shader->Uninit();

	for (auto shader : m_computeShaders)
		shader->Uninit();

	m_shaders.clear();
	m_computeShaders.clear();
	m_renderTargetViews.clear();

	SAFE_DELETE(m_viewPort);
	m_ImmediateContext->ClearState();
	m_RenderTargetView->Release();
	m_SwapChain->Release();
	m_ImmediateContext->Release();
	m_D3DDevice->Release();
}

void CRenderer::Begin(std::vector<uint8_t> renderTargetViews, bool clearBuffer, D3D11_VIEWPORT* viewPort, ID3D11DepthStencilView* depthStencilView)
{
	// get all the render targets to write to for this pass
	ID3D11RenderTargetView* renderTarget[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	ID3D11DepthStencilView* dsv;
	bool useDefault = false;
	for (int i = 0; i < renderTargetViews.size(); ++i)
	{
		if (renderTargetViews[i] == 1)
		{
			renderTarget[i] = m_RenderTargetView;
			useDefault = true;
		}
		else
			renderTarget[i] = m_renderTargetViews[renderTargetViews[i]]->GetRenderTargetView();
	}

	// set the right viewport for this pass
	if (useDefault || depthStencilView == nullptr || viewPort == nullptr)
	{
		m_ImmediateContext->RSSetViewports(1, m_viewPort);
		dsv = m_DepthStencilView;
	}
	else
	{
		m_ImmediateContext->RSSetViewports(1, viewPort);
		dsv = depthStencilView;
	}
	
	// set the render targets
	m_ImmediateContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, &renderTarget[0], dsv);

	// clear the render target buffer if needed
	if (!clearBuffer)
		return;

	float ClearColor[4] = { 0.3f, 0.3f, 1.0f, 1.0f };

	for (int i = 0; i < renderTargetViews.size(); ++i)
		m_ImmediateContext->ClearRenderTargetView(renderTarget[i], ClearColor);
	m_ImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CRenderer::End()
{
	m_SwapChain->Present( 1, 0 );
}

void CRenderer::SetDepthEnable( bool Enable )
{
	if( Enable )
		m_ImmediateContext->OMSetDepthStencilState( m_DepthStateEnable, NULL );
	else
		m_ImmediateContext->OMSetDepthStencilState( m_DepthStateDisable, NULL );

}

void CRenderer::SetShader(const std::shared_ptr<Shader>& shader)
{
	// return if the active shader is the same
	if (auto activeShader = m_activeShader.lock())
	{
		if (typeid(*shader) == typeid(*activeShader))
			return;
	}
	
	m_activeShader = shader;

	// �V�F�[�_�ݒ�
	m_ImmediateContext->VSSetShader(shader->m_vertexShader, NULL, 0);
	m_ImmediateContext->PSSetShader(shader->m_pixelShader, NULL, 0);

	shader->UpdateConstantBuffers();
}

void CRenderer::SetRasterizerState(RasterizerState state)
{
	switch (state)
	{
	case RasterizerState_CullBack:
		m_ImmediateContext->RSSetState(m_rasterizerCullBack);
		break;
	case RasterizerState_CullFront:
		m_ImmediateContext->RSSetState(m_rasterizerCullFront);
		break;
	case RasterizerState_Wireframe:
		m_ImmediateContext->RSSetState(m_rasterizerWireframe);
		break;
	default:
		break;
	}
}

std::shared_ptr<RenderTexture> CRenderer::GetRenderTexture(int renderTargetViewID)
{
	for (auto rtv : m_renderTargetViews)
	{
		if (rtv.second->GetRenderTargetViewID() == renderTargetViewID)
			return rtv.second;
	}

	return nullptr;
}

void CRenderer::DrawLine(const std::shared_ptr<Shader> shader, ID3D11Buffer** vertexBuffer, UINT vertexCount)
{
	// set the active shader
	SetShader(shader);

	// set vertex buffer
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);

	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//�|���S���`��
	CRenderer::GetDeviceContext()->Draw(vertexCount, 0);
}

void CRenderer::DrawPolygon(const std::shared_ptr<Shader> shader, ID3D11Buffer** vertexBuffer, UINT vertexCount)
{
	// set the active shader
	SetShader(shader);

	// set vertex buffer
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);

	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�|���S���`��
	CRenderer::GetDeviceContext()->Draw(vertexCount, 0);
}

void CRenderer::DrawPolygonIndexed(const std::shared_ptr<Shader> shader, ID3D11Buffer** vertexBuffer, ID3D11Buffer* indexBuffer, UINT indexCount)
{
	// set the active shader
	SetShader(shader);

	// set vertex and index buffers
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);
	CRenderer::GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�|���S���`��
	CRenderer::GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

void CRenderer::DrawModel(const std::shared_ptr<Shader> shader, const std::shared_ptr<Model> model, const bool loadTexture)
{
	// set the active shader
	SetShader(shader);

	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// loop for every mesh, set the corresponding textures and draw the model
	for (unsigned int m = 0; m < model->m_scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = model->m_scene->mMeshes[m];

		// set texture
		if (loadTexture)
		{
			aiMaterial* material = model->m_scene->mMaterials[mesh->mMaterialIndex];

			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			shader->SetTexture(model->m_texture[path.data]);
		}

		// set vertex buffer
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &model->m_vertexBuffer[m], &stride, &offset);

		// set index buffer
		CRenderer::GetDeviceContext()->IASetIndexBuffer(model->m_indexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// draw
		CRenderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

void CRenderer::DrawModelInstanced(const std::shared_ptr<Shader> shader, const std::shared_ptr<Model> model, int instanceCount)
{
	// set the active shader
	SetShader(shader);

	//�v���~�e�B�u�g�|���W�[�ݒ�
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// loop for every mesh, set the corresponding textures and draw the model
	for (unsigned int m = 0; m < model->m_scene->mNumMeshes; ++m)
	{
		aiMesh* mesh = model->m_scene->mMeshes[m];

		// set texture
		aiMaterial* material = model->m_scene->mMaterials[mesh->mMaterialIndex];

		aiString path;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		shader->SetTexture(model->m_texture[path.data]);

		// set vertex buffer
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &model->m_vertexBuffer[m], &stride, &offset);

		// set index buffer
		CRenderer::GetDeviceContext()->IASetIndexBuffer(model->m_indexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// draw
		CRenderer::GetDeviceContext()->DrawIndexedInstanced(mesh->mNumFaces * 3, instanceCount, 0, 0, 0);
	}
}

void CRenderer::BindRenderTargetView(const std::shared_ptr<RenderTexture>& renderTexture)
{
	m_renderTargetViews[renderTexture->GetRenderTargetViewID()] = renderTexture;
}

void CRenderer::UnbindRenderTargetViews()
{
	m_renderTargetViews.clear();
}
