
Texture2D g_Texture : register( t0 );
Texture2D g_MaskTexture : register(t1);

SamplerState g_SamplerState : register( s0 );

cbuffer ValueBuffer : register(b0)
{
    bool EnableTexture;
}

struct MATERIAL
{
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
};

cbuffer MaterialBuffer : register(b1)
{
    MATERIAL Material;
}

struct PixelOut
{
	float4 color : SV_Target;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(in float2 inScreenTexCoord    : TEXCOORD0,
			  in float4 inPosition          : SV_POSITION,
              in float2 inTexCoord          : TEXCOORD1)
{
    PixelOut pixel = (PixelOut) 0;
    
    // inside the portal
    if(EnableTexture)
    {
        inScreenTexCoord.xy /= inPosition.w;
        pixel.color = g_Texture.Sample(g_SamplerState, inScreenTexCoord);
    }
    else
        pixel.color.rgb = float3(0,0,0);
    
    // outer mask, edge color
    float4 maskColor = g_MaskTexture.Sample(g_SamplerState, inTexCoord);
    pixel.color.a = maskColor.a;
    
    if(maskColor.r >= 0.9f)
        pixel.color.rgb = Material.Diffuse.rgb;
    
    return pixel;
}
