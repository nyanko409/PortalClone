

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


// ライトバッファ
struct LIGHT
{
	bool		Enable;
	bool3		Dummy;//16byte境界用
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(	in  float4 inPosition	: POSITION0,
			in  float4 inNormal		: NORMAL0,
			in  float4 inDiffuse	: COLOR0,
			in  float2 inTexCoord	: TEXCOORD0,

			out float2 outTexCoord	: TEXCOORD0,
			out float4 outDiffuse	: COLOR0,
			out float4 outPosition	: SV_POSITION,
			out float4 outNormal	: NORMAL0,
			out float  outDepth		: DEPTH)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	outPosition = mul(inPosition, wvp);
	outNormal = inNormal;
	outTexCoord = inTexCoord;

	outDepth = 1 - saturate((50 - outPosition.w) / (50 - 0.1F));
	
	// lighting
	float4 worldNormal, normal;
	normal = float4(inNormal.xyz, 0.0);
	worldNormal = mul(normal, World);
	worldNormal = normalize(worldNormal);

	if (Light.Enable)
	{
		float light = 0.5 - 0.5 * dot(Light.Direction.xyz, worldNormal.xyz);

		outDiffuse = inDiffuse * Material.Diffuse * light * Light.Diffuse;
		outDiffuse += inDiffuse * Material.Ambient * Light.Ambient;
		outDiffuse += Material.Emission;
	}
	else
	{
		outDiffuse = inDiffuse * Material.Diffuse;
	}

	outDiffuse.a = inDiffuse.a * Material.Diffuse.a;
}
