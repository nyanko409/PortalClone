

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}
cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}
cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in  float4 inPosition		  : POSITION0,
		  
		  out float4 outPosition	  : SV_POSITION,
		  out float3 outWorldPosition : TEXCOORD0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	outPosition = mul( inPosition, wvp);
	outWorldPosition = mul(inPosition, World);
}
