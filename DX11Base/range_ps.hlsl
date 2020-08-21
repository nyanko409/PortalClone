cbuffer RangeBuffer : register(b0)
{
	float range;
	float3 playerPos;
}

cbuffer TimeBuffer : register(b1)
{
	float time;
}

Texture2D		g_Texture : register( t0 );
Texture2D		g_noiseTexture : register(t1);
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(
			in  float2 inTexCoord		: TEXCOORD0,
			in  float4 inDiffuse		: COLOR0,
			in  float4 inWorldPosition	: POSITION0,
			in  float4 inPosition		: SV_POSITION,
			in  float4 inNormal			: NORMAL0) : SV_Target
{
	float4 outDiffuse;

	float3 noise = g_noiseTexture.Sample(g_SamplerState, inTexCoord + time).xyz;
	
	float diff = length(abs(inWorldPosition.xyz - playerPos.xyz + noise));
	float width = abs(diff - range);

	if (diff < range)
	{
		outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
		outDiffuse *= inDiffuse;
	}
	else if(width < 0.2F)
	{
		outDiffuse.ra = 1;
		outDiffuse.gb = 0;
	}
	else
	{
		outDiffuse.rgba = 0;
	}

	return outDiffuse;
}
