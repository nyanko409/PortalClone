
cbuffer ProjectionBuffer : register(b0)
{
	matrix Projection;
}

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	//float3		Dummy;//16bit境界用
};

cbuffer MaterialBuffer : register(b1)
{
	MATERIAL	Material;
}



//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in  float4 inPosition		: POSITION0,
	in  float4 inNormal : NORMAL0,
	in  float4 inDiffuse : COLOR0,
	in  float2 inTexCoord : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	out float4 outNormal : NORMAL0,
	out float2 outTexCoord : TEXCOORD0,
	out float4 outDiffuse : COLOR0)
{
	outPosition = mul(inPosition, Projection);
	outNormal = inNormal;
	outTexCoord = inTexCoord;

	outDiffuse = inDiffuse * Material.Diffuse;
	outDiffuse.a = inDiffuse.a * Material.Diffuse.a;
}
