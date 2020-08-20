
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
	float4 outDiffuse;
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);

	outDiffuse *= inDiffuse;
	return outDiffuse;
}
