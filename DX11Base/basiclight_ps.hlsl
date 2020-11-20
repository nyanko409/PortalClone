
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );

struct PixelOut
{
	float4 color : SV_Target0;
	float4 color2 : SV_Target1;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(	in  float2 inTexCoord	: TEXCOORD0,
				in  float4 inDiffuse	: COLOR0,
				in  float4 inPosition	: SV_POSITION,
				in  float4 inNormal		: NORMAL0,
			    in  float  depth		: DEPTH)
{
	PixelOut pixel = (PixelOut)0;

    pixel.color = g_Texture.Sample( g_SamplerState, inTexCoord );
	pixel.color *= inDiffuse;

	//float4 d = inPosition;
	//outDiffuse.rgb = (2.0 * 0.1F) / (100 + 0.1F - d.z * (100 - 0.1F));
	pixel.color.rgb = lerp(float3(1,1,1), pixel.color.rgb, 1 - depth);
	
	pixel.color2.rgb = float3(0,0,1);
	pixel.color2.a = 1;

	return pixel;
}
