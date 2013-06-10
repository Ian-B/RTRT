uniform extern float4x4 g_matWorldViewProjection;
uniform extern float4x4 g_matWorldInverseTranspose;
uniform extern float4x4 g_matWorld;

uniform extern float3 g_vecLightDirection;
uniform extern float3 g_vecLightPosition;

uniform extern float g_fTheta;
uniform extern float g_fPhi;

//textures
uniform extern texture g_texture;

sampler textureSampler = sampler_state
{
	Texture = <g_texture>;
	MinFilter = LINEAR;
	magFilter = LINEAR;
	mipFilter = LINEAR;
};

struct VOUT
{
	float4 hposition : POSITION;
	float3 wposition : TEXCOORD0; // Position in view space  
	float3 wnormal   : TEXCOORD1; // Normal in view space  
	float2 uv	  	 : TEXCOORD2; // Texture coordinates
};

//vertext shader
VOUT LightingVS(float3 position : POSITION,
				float3 normal   : NORMAL,  
				float2 uv	  	: TEXCOORD0)					
{
	VOUT vout;
	vout.hposition = mul(float4(position, 1.0f), g_matWorldViewProjection);
	vout.wposition = mul(float4(position, 1.0f), g_matWorld); 
	vout.wnormal = normalize(mul(float4(normal, 0.0f), g_matWorldInverseTranspose)); 
	vout.uv = uv;  
	return vout;
}

//pixel shader
float4 LightingPS(VOUT IN) : COLOR
{ 
	float3 vLight = normalize(g_vecLightPosition - IN.wposition);  
	float NdotL = max(0.0f, dot(IN.wnormal, vLight)); 
	float cosAngle = max(0.0f, dot(-g_vecLightDirection, vLight));  
	float spotAtten = smoothstep(g_fPhi, g_fTheta, cosAngle); 
	float4 color = tex2D(textureSampler, IN.uv); 
	return float4((color*NdotL*spotAtten).xyz, 1.0f);
}

technique SpotLight
{
	pass	
	{		
		VertexShader = compile vs_2_0 LightingVS();	
		PixelShader = compile ps_2_0 LightingPS();
	}
}
