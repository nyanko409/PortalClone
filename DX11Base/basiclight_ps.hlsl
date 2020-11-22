
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );

// ライトバッファ
struct LIGHT
{
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

cbuffer LightBuffer : register(b0)
{
    LIGHT Light;
}

struct PixelOut
{
	float4 color : SV_Target0;
	float4 color2 : SV_Target1;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(	in  float2 inTexCoord	: TEXCOORD0,
				in  float4 inDiffuse	: COLOR0,
				in  float4 inPosition	: SV_POSITION,
				in  float4 inNormal		: NORMAL0,
			    in  float  depth		: DEPTH)
{
	PixelOut pixel = (PixelOut)0;

    // texture + vertex color
    pixel.color = g_Texture.Sample( g_SamplerState, inTexCoord );
	pixel.color *= inDiffuse;

    // fog
	pixel.color.rgb = lerp(float3(1,1,1), pixel.color.rgb, 1 - depth);
    
    // lighting
    inNormal = normalize(inNormal);
    float light = 0.5 - 0.5 * dot(Light.Direction.xyz, inNormal.xyz);

    pixel.color.rgb *= light * Light.Diffuse;
    pixel.color.rgb += Light.Ambient;

    pixel.color2.rgb = float3(0, 0, 1);
    pixel.color2.a = 1;
    
	return pixel;
}
