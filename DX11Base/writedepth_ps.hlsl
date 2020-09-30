

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(in float4 inPosition		: SV_POSITION,
			in float3 inWorldPosition	: TEXCOORD0)	: SV_Target
{
	float4 outDiffuse;
	outDiffuse.rgb = inWorldPosition.xyz * 0.01F;
	//outDiffuse.rgb = inPosition.z;
	outDiffuse.a = 1;

	return outDiffuse;
}
