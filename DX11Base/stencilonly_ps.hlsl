Texture2D g_MaskTexture : register(t0);
SamplerState g_SamplerState : register(s0);

struct PixelOut
{
	float4 color : SV_Target;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(  in float4 inPosition : SV_POSITION,
                in float2 inTexCoord : TEXCOORD0)
{
    // discard the pixel if its inside the portal
    float4 maskColor = g_MaskTexture.Sample(g_SamplerState, inTexCoord);
    if(maskColor.a <= 0.2f)
        discard;
    
    // write to stencil if its outside the portal
    PixelOut pixel = (PixelOut) 0;
    pixel.color.rgba = 0;
    
    return pixel;
}
