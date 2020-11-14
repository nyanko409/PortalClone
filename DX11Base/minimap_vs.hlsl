
cbuffer ProjectionBuffer : register(b0)
{
	matrix Projection;
}



//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(	in  float4 inPosition	: POSITION0,
			in  float4 inDiffuse	: COLOR0,
			in  float2 inTexCoord	: TEXCOORD0,

			out float2 outTexCoord	: TEXCOORD0,
			out float4 outDiffuse	: COLOR0,
			out float4 outPosition	: SV_POSITION)
{
	outPosition = mul(inPosition, Projection);
	outTexCoord = inTexCoord;
	outDiffuse = inDiffuse;
}
