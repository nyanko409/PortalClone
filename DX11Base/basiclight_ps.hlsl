
Texture2D g_Texture : register( t0 );
Texture2D g_ShadowMap : register(t1);

SamplerState g_SamplerState : register( s0 );
SamplerState g_ShadowMapSamler : register(s1);

struct LIGHT
{
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

struct MATERIAL
{
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
};

cbuffer LightBuffer : register(b0)
{
    LIGHT Light;
}

cbuffer MaterialBuffer : register(b1)
{
    MATERIAL Material;
}

cbuffer CameraBuffer : register(b2)
{
    float4 CameraPosition;
}

struct PixelOut
{
	float4 color : SV_Target;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(	in float2 inTexCoord	            : TEXCOORD0,
				in float4 inDiffuse	                : COLOR0,
				in float4 inPosition	            : SV_POSITION,
                in float4 outPositionInversePortal  : POSITION0)
{
    // portal clipping
    if(outPositionInversePortal.z < 0)
        discard;
    
    // texture + diffuse
    PixelOut pixel = (PixelOut) 0;
    
    pixel.color = g_Texture.Sample(g_SamplerState, inTexCoord);
    pixel.color *= inDiffuse;
    return pixel;
}
