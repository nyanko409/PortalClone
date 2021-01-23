Texture2D g_MaskTexture : register(t0);
SamplerState g_SamplerState : register(s0);

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
PixelOut main(in float4 inPosition : SV_POSITION,
                in float2 inTexCoord : TEXCOORD0)
{
    PixelOut pixel = (PixelOut) 0;
    pixel.color = float4(0, 0, 0, 0);
    
    // discard the pixel if its outside the portal
    float4 maskColor = g_MaskTexture.Sample(g_SamplerState, inTexCoord);
    if (maskColor.a <= 0.2f)
        discard;
    
    if(!EnableTexture)
        return pixel;
    
    // write to stencil if its inside the portal
    if (maskColor.r >= 0.9f)
    {
        pixel.color.rgb = Material.Diffuse.rgb;
        pixel.color.a = 1;
    }
    
    return pixel;
}
