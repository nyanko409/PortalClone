
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
PixelOut main(in float2 inTexCoord  : TEXCOORD0,
			  in float4 inPosition  : SV_POSITION)
{
    PixelOut pixel = (PixelOut) 0;
    
    if(EnableTexture)
    {
        inTexCoord.xy /= inPosition.w;
        pixel.color = g_Texture.Sample(g_SamplerState, inTexCoord);
    }
    else
        pixel.color = Material.Diffuse;
    
    //float4 maskColor = g_MaskTexture.Sample(g_SamplerState, inTexCoord);
    //pixel.color.a *= maskColor.a;
    
    return pixel;
}
