

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

cbuffer PortalInverseWorldBuffer : register(b3)
{
    matrix PortalInverseWorld;
    bool enableClip;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in float4 inPosition                  : POSITION0,
			in float4 inNormal                  : NORMAL0,
			in float4 inDiffuse                 : COLOR0,
			in float2 inTexCoord                : TEXCOORD0,
			out float2 outTexCoord              : TEXCOORD0,
			out float4 outDiffuse               : COLOR0,
			out float4 outPosition              : SV_POSITION,
            out float4 outPositionInversePortal : POSITION0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

    // portal clipping
    if(enableClip)
    {
        outPositionInversePortal = mul(inPosition, World);
        outPositionInversePortal = mul(outPositionInversePortal, PortalInverseWorld);
    }
    else
    {
        outPositionInversePortal = float4(0, 0, 1, 0);
    }
    
    // other stuff
	outTexCoord = inTexCoord;
    outDiffuse = inDiffuse;
}
