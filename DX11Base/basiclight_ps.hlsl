
Texture2D		g_Texture : register( t0 );
SamplerState	g_SamplerState : register( s0 );

struct LIGHT
{
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
};

struct MATERIAL
{
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
};

cbuffer LightBuffer : register(b0)
{
    LIGHT Light;
}

cbuffer MaterialBuffer : register(b1)
{
    MATERIAL Material;
}

cbuffer CameraBuffer : register(b2)
{
    float4 CameraPosition;
}

struct PixelOut
{
	float4 color : SV_Target0;
	float4 color2 : SV_Target1;
};

//=============================================================================
// ピクセルシェーダ
//=============================================================================
PixelOut main(	in float2 inTexCoord	    : TEXCOORD0,
				in float4 inDiffuse	        : COLOR0,
				in float4 inPosition	    : SV_POSITION,
				in float4 inNormal		    : NORMAL0,
                in float3 inWorldPosition   : TEXCOORD1,
			    in float  depth		        : DEPTH)
{
	PixelOut pixel = (PixelOut)0;

    // texture + vertex color
    pixel.color = g_Texture.Sample( g_SamplerState, inTexCoord );
	pixel.color *= inDiffuse * Material.Diffuse;

    // lighting
    inNormal = normalize(inNormal);
    float light = 0.5 - 0.5 * dot(Light.Direction.xyz, inNormal.xyz);

    pixel.color.rgb *= light * Light.Diffuse;
    pixel.color.rgb += Light.Ambient;
    
    // specular blinn-phong
    float3 eyev = inWorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
	
    float3 halfv = eyev + Light.Direction.xyz;
    halfv = normalize(halfv);
	
    float specular = -dot(halfv, inNormal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 60);
	
    pixel.color.rgb += specular * Material.Specular;
    
    // fog
	//pixel.color.rgb = lerp(float3(1,1,1), pixel.color.rgb, 1 - depth);
    
    pixel.color2.rgba = float4(0, 0, 1, 1);
	return pixel;
}
