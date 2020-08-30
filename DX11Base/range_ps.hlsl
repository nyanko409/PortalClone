cbuffer RangeBuffer : register(b0)
{
	float range;
	float3 playerPos;
	float range2;
	float3 playerPos2;
}

cbuffer TimeBuffer : register(b1)
{
	float time;
}

cbuffer ValueBuffer : register(b3)
{
	float uvScale;
	bool enableNormal;
	bool invisibleOutside;
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

	// get noise from texture
	float3 noise = g_noiseTexture.Sample(g_SamplerState, (inWorldPosition.xz * 0.05F) + time).xyz;
	
	// range cutoff 1
	float diff = length(abs(inWorldPosition.xyz - playerPos.xyz + noise));
	float width = abs(diff - range);

	// range cutoff 2
	float diff2 = length(abs(inWorldPosition.xyz - playerPos2.xyz + noise));
	float width2 = abs(diff2 - range2);

	if (diff < range || diff2 < range2)
	{
		// in range
		outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord * uvScale);
		outDiffuse *= inDiffuse;

		// lighting
		if (Light.Enable)
		{
			// get normal map data
			float3 bumpNormal = inNormal;
			if (enableNormal)
			{
				float4 bumpMap = g_normalTexture.Sample(g_SamplerState, inTexCoord * uvScale);
				bumpMap = (bumpMap * 2.0F) - 1.0F;
				bumpNormal = (bumpMap.x * inTangent) + (bumpMap.y * inBinormal) + (bumpMap.z * inNormal);
				bumpNormal = normalize(bumpNormal);
			}
			
			// calculate lighting
			float3 direction = -Light.Direction;
			float lightIntensity = saturate(dot(bumpNormal, direction));
			float4 color = saturate(Light.Diffuse * lightIntensity);
			outDiffuse.rgb *= color.rgb;
		}
	}
	else if(width < 0.2F || width2 < 0.2F)
	{
		// color the edge
		outDiffuse.ra = 1;
		outDiffuse.gb = 0;
	}
	else
	{
		// out of range
		if (!invisibleOutside)
		{
			outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord * (uvScale - 0.02F));
			outDiffuse *= inDiffuse;
			outDiffuse.rgb *= 0.3F;
		}
		else
			outDiffuse.rgba = 0;
	}

	return outDiffuse;
}
