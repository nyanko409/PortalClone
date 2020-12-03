

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

StructuredBuffer<float3> Position : register(t2);


//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(in float4 inPosition          : POSITION0,
			in float4 inNormal          : NORMAL0,
			in float4 inDiffuse         : COLOR0,
			in float2 inTexCoord        : TEXCOORD0,
            in uint inInstanceId        : SV_InstanceID,

			out float2 outTexCoord      : TEXCOORD0,
			out float4 outDiffuse       : COLOR0,
			out float4 outPosition      : SV_POSITION,
			out float4 outNormal        : NORMAL0,
            out float3 outWorldPosition : TEXCOORD1)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
    inPosition.xyz += Position[inInstanceId];
	outPosition = mul(inPosition, wvp);
    
	outTexCoord = inTexCoord;
    outDiffuse = inDiffuse;
    outWorldPosition = mul(inPosition, World);
    
    float4 normal;
    normal = float4(inNormal.xyz, 0.0);
    normal = mul(normal, World);
    outNormal = normalize(normal);

	// linear fog
	//outDepth = 1 - saturate((80 - outPosition.w) / (80 - 0.1F));
	
	// exponential fog
	//outDepth = 1 - pow(1.0 / 2.71828, outPosition.w * 0.05F);
}
