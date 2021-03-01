Texture2D g_MaskTexture : register(t0);
SamplerState g_SamplerState : register(s0);

struct MATERIAL
{
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
};

cbuffer MaterialBuffer : register(b0)
{
    MATERIAL Material;
}

cbuffer ValueBuffer : register(b1)
{
    bool StencilPass;
}

struct PixelOut
{
	float4 color : SV_Target;
};

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
PixelOut main(  in float4 inPosition : SV_POSITION,
                in float2 inTexCoord : TEXCOORD0)
{
    PixelOut pixel = (PixelOut)0;
    if(StencilPass)
    {
        pixel.color.rgba = 0;
        return pixel;
    }
    
    pixel.color = g_MaskTexture.Sample(g_SamplerState, inTexCoord);
    if(pixel.color.a < 0.1f)
        discard;
    
    pixel.color.rgb = Material.Diffuse;
    
    return pixel;
}
