
Texture2D g_Texture : register( t0 );
SamplerState g_SamplerState : register( s0 );

cbuffer CameraBuffer : register(b0)
{
    float4 CameraPosition;
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
	float4 color : SV_Target0;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(in float2 inTexCoord  : TEXCOORD0,
			  in float4 inPosition  : SV_POSITION,
			  in float4 inNormal    : NORMAL0)
{
    PixelOut pixel = (PixelOut) 0;
    pixel.color = Material.Diffuse;
    
    return pixel;
}
