
struct PixelOut
{
	float4 color : SV_Target0;
};

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
PixelOut main(in float4 inPosition	        : SV_POSITION,
			  in float  depth		        : DEPTH)
{
	PixelOut pixel = (PixelOut)0;
    //pixel.color.rgb = depth;
    pixel.color.rgb = inPosition.z;
    pixel.color.a = 1;

	return pixel;
}
