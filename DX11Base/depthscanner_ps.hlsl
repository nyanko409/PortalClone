
cbuffer ValueBuffer : register(b0)
{
	bool enableTexture;
}

Texture2D		g_Texture : register(t0);
Texture2D		g_depthTexture : register(t1);
Texture2D		g_minimapTexture : register(t2);
SamplerState	g_SamplerState : register(s0);


//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(
	in  float2 inTexCoord	: TEXCOORD0,
	in  float4 inDiffuse	: COLOR0,
	in  float4 inPosition	: SV_POSITION)		: SV_Target
{
	float4 outDiffuse = inDiffuse;

	if (enableTexture)
	{
		outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
		outDiffuse *= inDiffuse;

		float4 minimap = g_minimapTexture.Sample(g_SamplerState, inTexCoord);

		if (minimap.a == 0)
			discard;

		if (minimap.r < 0.9F)
		{
			outDiffuse.rgb = minimap.rgb;
		}

		//float4 depthValue = g_depthTexture.Sample(g_SamplerState, inTexCoord);
		//float range = length(depthValue.xyz);
		//if (range < 0.01F)
		//{
		//	outDiffuse.rgb = 0;
		//}
	}

	return outDiffuse;
}
