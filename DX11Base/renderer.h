#pragma once

#include <typeinfo>


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
class Shader;


class CRenderer
{
private:
	static D3D_FEATURE_LEVEL       m_FeatureLevel;

	static ID3D11Device*           m_D3DDevice;
	static ID3D11DeviceContext*    m_ImmediateContext;
	static IDXGISwapChain*         m_SwapChain;
	static ID3D11RenderTargetView* m_RenderTargetView;
	static ID3D11DepthStencilView* m_DepthStencilView;

	static ID3D11DepthStencilState* m_DepthStateEnable;
	static ID3D11DepthStencilState* m_DepthStateDisable;

	static std::vector<Shader*> m_shaders;
	static Shader* m_activeShader;

public:
	static void Init();
	static void Uninit();
	static void Begin();
	static void End();

	static void SetDepthEnable(bool Enable);
	static void SetShader(Shader* shader);

	template <typename T>
	static T* GetShader()
	{
		for (Shader* shader : m_shaders)
		{
			if (typeid(*shader) == typeid(T))
				return (T*)shader;
		}

		return nullptr;
	}
	static std::vector<Shader*> GetShaders() { return m_shaders; }

	static ID3D11Device* GetDevice(){ return m_D3DDevice; }
	static ID3D11DeviceContext* GetDeviceContext(){ return m_ImmediateContext; }
};
