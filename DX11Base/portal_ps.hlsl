
Texture2D g_Texture : register( t0 );
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
PixelOut main(in float2 inTexCoord  : TEXCOORD0,
			  in float4 inPosition  : SV_POSITION,
			  in float4 inNormal    : NORMAL0)
{
    PixelOut pixel = (PixelOut) 0;
    
    if(EnableTexture)
        pixel.color = g_Texture.Sample(g_SamplerState, inTexCoord);
    else
        pixel.color = Material.Diffuse;
    
    return pixel;
}
