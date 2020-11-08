
Texture2D		g_Texture : register(t0);
Texture2D		g_minimapTexture : register(t1);
SamplerState	g_SamplerState : register(s0);


//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(
	in  float2 inTexCoord	: TEXCOORD0,
	in  float4 inDiffuse	: COLOR0,
	in  float4 inPosition	: SV_POSITION)		: SV_Target
{
	float4 outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
	outDiffuse *= inDiffuse;

	float4 minimap = g_minimapTexture.Sample(g_SamplerState, inTexCoord);

	if (minimap.a == 0)
		discard;

	if (minimap.r < 0.9F)
	{
		outDiffuse.rgb = minimap.rgb;
	}

	return outDiffuse;
}
