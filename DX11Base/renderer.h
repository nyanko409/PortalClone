#pragma once

#include <typeinfo>


// 頂点構造体
struct VERTEX_3D
{
    dx::XMFLOAT3 Position;
	dx::XMFLOAT3 Normal;
	dx::XMFLOAT4 Diffuse;
	dx::XMFLOAT2 TexCoord;
	dx::XMFLOAT3 Tangent;
	dx::XMFLOAT3 Binormal;
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

enum RasterizerState
{
	RasterizerState_CullBack,
	RasterizerState_CullFront,
	RasterizerState_Wireframe
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
class ComputeShader;
class Model;


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

	static ID3D11RasterizerState* m_rasterizerCullBack;
	static ID3D11RasterizerState* m_rasterizerCullFront;
	static ID3D11RasterizerState* m_rasterizerWireframe;

	static std::vector<std::shared_ptr<Shader>> m_shaders;
	static std::weak_ptr<Shader> m_activeShader;

	static std::vector<std::shared_ptr<ComputeShader>> m_computeShaders;

public:
	static void Init();
	static void Uninit();
	static void Begin();
	static void End();

	static void SetDepthEnable(bool Enable);
	static void SetShader(const std::shared_ptr<Shader>& shader);

	template <typename T>
	static std::shared_ptr<T> GetShader()
	{
		for (auto shader : m_shaders)
		{
			if (typeid(*shader) == typeid(T))
				return std::dynamic_pointer_cast<T>(shader);
		}

		return nullptr;
	}
	static std::vector<std::shared_ptr<Shader>> GetShaders() { return m_shaders; }

	template <typename T>
	static std::shared_ptr<T> GetComputeShader()
	{
		for (auto shader : m_computeShaders)
		{
			if (typeid(*shader) == typeid(T))
				return std::dynamic_pointer_cast<T>(shader);
		}

		return nullptr;
	}

	static ID3D11Device* GetDevice(){ return m_D3DDevice; }
	static ID3D11DeviceContext* GetDeviceContext(){ return m_ImmediateContext; }
	static void SetRasterizerState(RasterizerState state);
	static void DrawModel(const std::shared_ptr<Shader> shader, const std::shared_ptr<Model> model);
	static void DrawPolygon(const std::shared_ptr<Shader> shader, ID3D11Buffer** vertexBuffer, UINT vertexCount);
	static void DrawPolygonIndexed(const std::shared_ptr<Shader> shader, ID3D11Buffer** vertexBuffer, ID3D11Buffer* indexBuffer, UINT indexCount);
};
