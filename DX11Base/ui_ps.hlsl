
struct MATERIAL
{
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
};

cbuffer MaterialBuffer : register(b0)
{
    MATERIAL Material;
}

cbuffer ValueBuffer : register(b1)
{
	bool enableTexture;
}

Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);


//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(
	in  float2 inTexCoord	: TEXCOORD0,
	in  float4 inDiffuse	: COLOR0,
	in  float4 inPosition	: SV_POSITION)		: SV_Target
{
	float4 outDiffuse = inDiffuse * Material.Diffuse;

	if (enableTexture)
	{
		outDiffuse *= g_Texture.Sample(g_SamplerState, inTexCoord);
	}

	return outDiffuse;
}
