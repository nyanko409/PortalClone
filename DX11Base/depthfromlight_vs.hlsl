

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

struct LIGHT
{
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

cbuffer LightBuffer : register(b3)
{
    LIGHT Light;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in float4 inPosition          : POSITION0,
			in float4 inNormal          : NORMAL0,
			in float4 inDiffuse         : COLOR0,
			in float2 inTexCoord        : TEXCOORD0,

			out float4 outPosition      : SV_POSITION,
			out float  outDepth		    : DEPTH)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
    
	outPosition = mul(inPosition, wvp);

	// linear depth
	outDepth = 1 - saturate((80 - outPosition.w) / (80 - 0.1F));
}
