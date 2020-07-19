#pragma once


// 頂点構造体
struct VERTEX_3D
{
    dx::XMFLOAT3 Position;
	dx::XMFLOAT3 Normal;
	dx::XMFLOAT4 Diffuse;
	dx::XMFLOAT2 TexCoord;
};



// マテリアル構造体
struct MATERIAL
{
	dx::XMFLOAT4	Ambient;
	dx::XMFLOAT4	Diffuse;
	dx::XMFLOAT4	Specular;
	dx::XMFLOAT4	Emission;
	float			Shininess;
	float			Dummy[3];	//16byte境界用
};




// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL		Material;
	class CTexture*	Texture;
};


// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct LIGHT
{
	BOOL			Enable;
	BOOL			Dummy[3];	//16byte境界用
	dx::XMFLOAT4	Direction;
	dx::XMFLOAT4	Diffuse;
	dx::XMFLOAT4	Ambient;
};


class CVertexBuffer;
class CIndexBuffer;
class CTexture;


class CRenderer
{
private:
	
	static D3D_FEATURE_LEVEL       m_FeatureLevel;

	static ID3D11Device*           m_D3DDevice;
	static ID3D11DeviceContext*    m_ImmediateContext;
	static IDXGISwapChain*         m_SwapChain;
	static ID3D11RenderTargetView* m_RenderTargetView;
	static ID3D11DepthStencilView* m_DepthStencilView;



	static ID3D11VertexShader*     m_VertexShader;
	static ID3D11PixelShader*      m_PixelShader;
	static ID3D11InputLayout*      m_VertexLayout;
	static ID3D11Buffer*			m_WorldBuffer;
	static ID3D11Buffer*			m_ViewBuffer;
	static ID3D11Buffer*			m_ProjectionBuffer;
	static ID3D11Buffer*			m_MaterialBuffer;
	static ID3D11Buffer*			m_LightBuffer;


	static ID3D11DepthStencilState* m_DepthStateEnable;
	static ID3D11DepthStencilState* m_DepthStateDisable;




public:
	static void Init();
	static void Uninit();
	static void Begin();
	static void End();

	static void SetDepthEnable(bool Enable);
	static void SetWorldMatrix(dx::XMMATRIX * WorldMatrix);
	static void SetViewMatrix(dx::XMMATRIX * ViewMatrix);
	static void SetProjectionMatrix(dx::XMMATRIX * ProjectionMatrix);
	static void SetMaterial(MATERIAL Material);
	static void SetLight(LIGHT Light);

	static ID3D11Device* GetDevice( void ){ return m_D3DDevice; }
	static ID3D11DeviceContext* GetDeviceContext( void ){ return m_ImmediateContext; }
};
