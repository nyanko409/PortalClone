
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
PixelOut main(	in float2 inTexCoord	    : TEXCOORD0,
				in float4 inDiffuse	        : COLOR0,
				in float4 inPosition	    : SV_POSITION,
				in float4 inNormal		    : NORMAL0,
                in float3 inWorldPosition   : TEXCOORD1,
                in float4 inLightPosition   : POSITION0,
			    in float  depth		        : DEPTH)
{
    PixelOut pixel = (PixelOut) 0;
    
    pixel.color = g_Texture.Sample(g_SamplerState, inTexCoord);
    pixel.color *= inDiffuse;
    return pixel;
    
    /*
    // handle shadow (percentage closer filtering PCF)
    inLightPosition.xyz /= inLightPosition.w;
    inLightPosition.x = inLightPosition.x * 0.5f + 0.5f;
    inLightPosition.y = -inLightPosition.y * 0.5f + 0.5f;
    
    if(inLightPosition.x > 1 || inLightPosition.x < 0 || inLightPosition.y > 1 || inLightPosition.y < 0)
        return pixel;
    
    // texel to uv offset
    float xOffset = 1.0f / 1080.0f;
    float yOffset = 1.0f / 720.0f;
    
    // get the average from neighbouring texels
    float average = 0;
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            float2 offset = float2(x * xOffset, y * yOffset);
            if (g_ShadowMap.Sample(g_ShadowMapSamler, inLightPosition.xy + offset).r < inLightPosition.z - 0.01f)
            {
                average++;
            }
        }
    }
    
    average /= 9.0f;
    if (average >= 0.1f)
        pixel.color.rgb *= (1 - average) + 0.2f;
    
    return pixel;
*/
}
