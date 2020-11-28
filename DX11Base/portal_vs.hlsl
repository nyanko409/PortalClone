

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
void main(in float4 inPosition : POSITION0,
			in float4 inNormal : NORMAL0,
			in float4 inDiffuse : COLOR0,
			in float2 inTexCoord : TEXCOORD0,

			out float2 outTexCoord : TEXCOORD0,
			out float4 outPosition : SV_POSITION,
			out float4 outNormal : NORMAL0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);
    
	outTexCoord = inTexCoord;
    
    float4 normal;
    normal = float4(inNormal.xyz, 0.0);
    normal = mul(normal, World);
    outNormal = normalize(normal);
}
