struct PixelOut
{
	float4 color : SV_Target;
};

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
PixelOut main(in float4 inPosition : SV_POSITION)
{
    //discard;
    PixelOut pixel = (PixelOut) 0;
    pixel.color.rgba = 0;
    return pixel;
}
