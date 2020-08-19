cbuffer RangeBuffer : register(b0)
{
	float range;
	float3 playerPos;
}

Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main( in  float4 inPosition	 : POSITION0,
		   in  float4 inNormal		 : NORMAL0,
		   in  float2 inTexCoord	 : TEXCOORD0,
		   in  float4 inDiffuse		 : COLOR0,
		   in float4 inWorldPosition : POSITION1) : SV_Target
{
	float4 outDiffuse;

	if (length(abs(inWorldPosition.xyz - playerPos.xyz)) > range)
	{
		outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
		outDiffuse *= inDiffuse;
	}
	else
	{
		outDiffuse.rgba = 0;
	}

	return outDiffuse;
}
