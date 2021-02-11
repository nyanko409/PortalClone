
struct PixelOut
{
	float4 color : SV_Target0;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(in float4 inPosition : SV_POSITION)
{
	PixelOut pixel = (PixelOut)0;
    pixel.color.rgba = 1;
    
	return pixel;
}
