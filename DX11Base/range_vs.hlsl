

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}
cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}
cbuffer ProjectionBuffer : register( b2 )
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
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(				in float4 inPosition			: POSITION0,
						in float4 inNormal				: NORMAL0,
						in float4 inDiffuse				: COLOR0,
						in float2 inTexCoord			: TEXCOORD0,
						in float3 inTangent				: TANGENT,
						in float3 inBinormal			: BINORMAL,

						out float2 outTexCoord			: TEXCOORD0,
						out float4 outDiffuse			: COLOR0,
						out float4 outWorldPosition		: POSITION0,
						out float4 outPosition			: SV_POSITION,
						out float4 outNormal			: NORMAL0,
						out float3 outTangent			: TANGENT,
						out float3 outBinormal			: BINORMAL)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	outPosition = mul( inPosition, wvp);
	outTexCoord = inTexCoord;
	
	// set normal
	float4 worldNormal, normal;
	normal = float4(inNormal.xyz, 0);
	worldNormal = mul(normal, World);
	worldNormal = normalize(worldNormal);
	outNormal = worldNormal;

	// set diffuse
	outDiffuse = inDiffuse * Material.Diffuse;
	outDiffuse.a = inDiffuse.a * Material.Diffuse.a;
	
	// set out world without vp
	float4 pos;
	pos = mul(inPosition, World);
	outWorldPosition.xyz = pos.xyz;

	// tangent and binormal for normal mapping
	outTangent = mul(inTangent, (float3x3)World);
	outTangent = normalize(outTangent);

	outBinormal = mul(inBinormal, (float3x3)World);
	outBinormal = normalize(outBinormal);
}
