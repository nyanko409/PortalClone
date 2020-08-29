cbuffer RangeBuffer : register(b0)
{
	float range;
	float3 playerPos;
}

cbuffer TimeBuffer : register(b1)
{
	float time;
}

struct LIGHT
{
	bool		Enable;
	bool3		Dummy;
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
};

cbuffer LightBuffer : register(b2)
{
	LIGHT		Light;
}

Texture2D		g_Texture : register( t0 );
Texture2D		g_noiseTexture : register(t1);
Texture2D		g_normalTexture : register(t2);
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(
			in float2 inTexCoord		: TEXCOORD0,
			in float4 inDiffuse			: COLOR0,
			in float4 inWorldPosition	: POSITION0,
			in float4 inPosition		: SV_POSITION,
			in float4 inNormal			: NORMAL0,
			in float3 inTangent			: TANGENT,
			in float3 inBinormal		: BINORMAL) : SV_Target
{
	// init
	float4 outDiffuse;
	float uvScale = 5;

	// range cutoff
	float3 noise = g_noiseTexture.Sample(g_SamplerState, inTexCoord + time).xyz;
	
	float diff = length(abs(inWorldPosition.xyz - playerPos.xyz + noise));
	float width = abs(diff - range);

	if (diff < range)
	{
		outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord * uvScale);
		outDiffuse *= inDiffuse;

		// lighting
		if (Light.Enable)
		{
			// normal
			float4 bumpMap = g_normalTexture.Sample(g_SamplerState, inTexCoord * uvScale);
			bumpMap = (bumpMap * 2.0F) - 1.0F;
			float3 bumpNormal = (bumpMap.x * inTangent) + (bumpMap.y * inBinormal) + (bumpMap.z * inNormal);
			bumpNormal = normalize(bumpNormal);

			float3 direction = -Light.Direction;
			float lightIntensity = saturate(dot(bumpNormal, direction));
			float4 color = saturate(Light.Diffuse * lightIntensity);
			outDiffuse.rgb *= color.rgb;
		}
	}
	else if(width < 0.2F)
	{
		// color the edge
		outDiffuse.ra = 1;
		outDiffuse.gb = 0;
	}
	else
	{
		// out of range
		outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
		outDiffuse *= inDiffuse;
		outDiffuse.rgb *= 0.3F;
	}

	return outDiffuse;
}
