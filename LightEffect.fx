// matrices
uniform extern float4x4 g_matWorldViewProjection;
uniform extern float4x4 g_matWorldInverseTranspose;
uniform extern float4x4 g_matWorld;
uniform extern float4x4 g_matView;
uniform extern float4x4 g_matLightWVP;

uniform extern float4x4 g_matBillboardTrans; //billboarding

// material components
uniform extern float4 g_vecMaterialDiffuse;
uniform extern float4 g_vecMaterialAmbient;
uniform extern float4 g_vecMaterialSpecular;

// light components
uniform extern float4 g_vecLightDiffuse;
uniform extern float4 g_vecLightAmbient;
uniform extern float4 g_vecLightSpecular;

//light vector
uniform extern float3 g_vecLightDirection;

// camera compoenents
uniform extern float3 g_vecCameraPos;

//specular power
uniform extern float g_fSpecPower;

//ambient levels
uniform extern float g_fAmbients;

//displacment value
uniform extern float g_fDisplaceVal;

//textures
uniform extern texture g_texture;
uniform extern texture g_textureTree;

uniform extern texture	g_textureDisp;	// displacement texture
//uniform extern float	g_fDispFactor;	// displacement factor

//normal map texture
uniform extern texture g_normalMap;

// shadow map components
uniform extern texture g_texShadowMap;

//spotlights
uniform extern float3	g_vecAttenuation012;

//spotlight1
uniform extern float	g_fSpotlightPower1;
uniform extern float3	g_vecSpotlightPos1;
uniform extern float3	g_vecSpotlightDirection1;
uniform extern float3	g_vecSpotlightColour1;

//spotlight2
uniform extern float	g_fSpotlightPower2;
uniform extern float3	g_vecSpotlightPos2;
uniform extern float3	g_vecSpotlightDirection2;
uniform extern float3	g_vecSpotlightColour2;

// texture sampler
sampler textureSampler = sampler_state
{
	Texture = <g_texture>;
	MinFilter = LINEAR;
	magFilter = LINEAR;
	mipFilter = LINEAR;
};

//terrain texture sampler
sampler textureSamplerTerrain = sampler_state
{
	Texture = <g_texture>;
	MinFilter = LINEAR;
	magFilter = LINEAR;
	mipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// normalmap texture sampler
sampler sampNormalMap = sampler_state
{
	Texture = <g_normalMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

// displacment texture sampler
sampler DispSampler = sampler_state
{
    Texture   = <g_textureDisp>;
    MipFilter = NONE;
    MinFilter = POINT;
    MagFilter = POINT;
};

sampler TexSamplerWrap = sampler_state
{
	Texture = <g_textureTree>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

//shaddow map 
sampler g_sampShadowMap = sampler_state
{
	texture = <g_texShadowMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
};


struct VSPhongInputTerrain
{
	float3 pos: POSITION0;
	float3 norm: NORMAL0;
	float2 texCoord: TEXCOORD0;
	//normal map
	float3 tan: TANGENT0;
	float3 bin: BINORMAL0;

};

struct VSPhongOutputTerrain
{
	float4 pos: POSITION0;
	float3 norm: TEXCOORD0;
	float3 posW: TEXCOORD1;
	float2 texCoord	: TEXCOORD2;
	float3 lightDir: TEXCOORD3;
	float4 projTex: TEXCOORD4;
};

// vertex shader input structure
struct VSInput_PosTex
{
	float3 pos: POSITION0;
	float2 tex: TEXCOORD0;
};

// vertex shader output structure
struct VSOutput_PosTex
{
	float4 pos: POSITION0;
	float2 tex: TEXCOORD0;
	float4 projTex: TEXCOORD1;
};

//--------- Phong Lighting Shaders ---------

//vertex shader Terrain
VSPhongOutputTerrain VS_PhongTerrain(VSPhongInputTerrain a_Input)
{
	VSPhongOutputTerrain Output;

	// retrieve offset value
	float g_fDisplaceVal = tex2Dlod(DispSampler, float4(a_Input.texCoord.x, a_Input.texCoord.y, 0, 0)).x;

	// displace vertex along normal
	float3 displacePos = a_Input.pos + a_Input.norm * g_fDisplaceVal * 2.0; //g_fDispFactor;

	// set vertex position
	Output.pos = mul(float4(displacePos, 1.0f), g_matWorldViewProjection);

	Output.posW = mul(float4(a_Input.pos, 1.0f), g_matWorld).xyz;

	float3 normal = mul(float4(a_Input.norm, 0.0f), g_matWorldInverseTranspose).xyz;
	Output.norm = normalize(normal);

	Output.texCoord = a_Input.texCoord;

	//shadow map 
	Output.projTex = mul(float4(a_Input.pos, 1.0f), g_matLightWVP);
	
	//convert to tangent space
	float3x3 toTangentSpace = transpose(float3x3(a_Input.tan, a_Input.bin, a_Input.norm));

	// calculate light's direction relative to tangent space
	float3 lightDirL = mul(float4(g_vecLightDirection, 0.0f), g_matWorldInverseTranspose).xyz;
	Output.lightDir = mul(lightDirL, toTangentSpace);

	return Output;
}

//pixel shader
float4 PS_PhongNormalTerrain(VSPhongOutputTerrain a_Output) : COLOR
{
	a_Output.norm = normalize(a_Output.norm);

	float3 light = normalize(a_Output.lightDir); //normalize light vec
	float4 colorTexture = tex2D(textureSampler, a_Output.texCoord); //texture
	float4 normalTexture = tex2D(sampNormalMap, a_Output.texCoord); //normal map
	normalTexture = (normalTexture - 0.5f) * 2.0f; //from -1 to 1
	
	float diffuseMap = max(dot(normalTexture, light), 0.0f); //material diffuse

	float3 toEye = normalize(g_vecCameraPos - a_Output.posW);

	float3 r = reflect(-g_vecLightDirection, a_Output.norm);

	float t = pow(max(dot(r, toEye), 0.0f), g_fSpecPower);

	float s = max(dot(g_vecLightDirection, a_Output.norm), 0.0f);
	
	//calculate new diffuse value based on normal map
	float3 newDiffuse = (g_vecMaterialDiffuse*diffuseMap);

	float3 newLAmbients = (g_vecLightAmbient * g_fAmbients); //levels

	float3 specular = t * (g_vecLightSpecular * g_vecMaterialSpecular).rgb;
	float3 diffuse = s * (g_vecLightDiffuse * newDiffuse).rgb;
	float3 ambient = (newLAmbients * g_vecMaterialAmbient);

	//shadow map
	// transform coordinates into texture coordinates
	a_Output.projTex.xy /= a_Output.projTex.w;            
	a_Output.projTex.x =  0.5f*a_Output.projTex.x + 0.5f; 
	a_Output.projTex.y = -0.5f*a_Output.projTex.y + 0.5f;
	
	// Compute pixel depth for shadowing.
	float depth = a_Output.projTex.z / a_Output.projTex.w;
 
	// Transform to texel space
    float2 texelpos = 512.0f * a_Output.projTex.xy;
        
    // Determine the lerp amounts.           
    float2 lerps = frac( texelpos );
    
    // sample shadow map
    float dx = 1.0f / 512.0f;
	float s0 = (tex2D(g_sampShadowMap, a_Output.projTex.xy).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s1 = (tex2D(g_sampShadowMap, a_Output.projTex.xy + float2(dx, 0.0f)).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s2 = (tex2D(g_sampShadowMap, a_Output.projTex.xy + float2(0.0f, dx)).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s3 = (tex2D(g_sampShadowMap, a_Output.projTex.xy + float2(dx, dx)).r   + 0.00005f < depth) ? 0.0f : 1.0f;

	float shadowCoeff = 0.0f;
	
	shadowCoeff = lerp( lerp( s0, s1, lerps.x ), lerp( s2, s3, lerps.x ), lerps.y );

	//------------------------------------------------------------------------
	// spotlight1
	//------------------------------------------------------------------------
	// Transform vertex position to world space.
	// Unit vector from vertex to light source.
	float3 lightVecW = normalize(g_vecSpotlightPos1 - a_Output.posW);

	float spot1 = pow(max(dot(-lightVecW, g_vecSpotlightDirection1), 0.0f), g_fSpotlightPower1);
	float3 spotlight1 = spot1*(diffuse + ambient + specular + g_vecSpotlightColour1);
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// spotlight2
	//------------------------------------------------------------------------
	// Transform vertex position to world space.
	// Unit vector from vertex to light source.
	lightVecW = normalize(g_vecSpotlightPos2 - a_Output.posW);

	float spot2 = pow(max(dot(-lightVecW, g_vecSpotlightDirection2), 0.0f), g_fSpotlightPower2);
	float3 spotlight2 = spot2*(diffuse + ambient + specular + g_vecSpotlightColour2);
	//------------------------------------------------------------------------

	return float4(spotlight1 + spotlight2 + ((diffuse + ambient + specular + colorTexture) * shadowCoeff), g_vecMaterialDiffuse.a);
}

// position and texture vertex shader
VSOutput_PosTex VS_Texture(VSInput_PosTex a_Input)
{
	VSOutput_PosTex Output;

	// perform billboarding orientation
	Output.pos = mul(a_Input.pos, transpose(g_matView));
	Output.pos.w = 1.0f;

	// translate next tree
	Output.pos = mul(Output.pos, g_matBillboardTrans);

	// compute vertex transformation
	Output.pos = mul(Output.pos, g_matWorldViewProjection);

	// pass texture coordinates through
	Output.tex = a_Input.tex;

	//shadow map 
	Output.projTex = mul(float4(a_Input.pos, 1.0f), g_matLightWVP);

	return Output;
}

// pixel shader that performs a simple texture lookup
float4 PS_Texture(VSOutput_PosTex a_Input) : COLOR
{
	//shadow map
	// transform coordinates into texture coordinates
	a_Input.projTex.xy /= a_Input.projTex.w;            
	a_Input.projTex.x =  0.5f*a_Input.projTex.x + 0.5f; 
	a_Input.projTex.y = -0.5f*a_Input.projTex.y + 0.5f;
	
	// Compute pixel depth for shadowing.
	float depth = a_Input.projTex.z / a_Input.projTex.w;
 
	// Transform to texel space
    float2 texelpos = 512.0f * a_Input.projTex.xy;
        
    // Determine the lerp amounts.           
    float2 lerps = frac( texelpos );
    
    // sample shadow map
    float dx = 1.0f / 512.0f;
	float s0 = (tex2D(g_sampShadowMap, a_Input.projTex.xy).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s1 = (tex2D(g_sampShadowMap, a_Input.projTex.xy + float2(dx, 0.0f)).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s2 = (tex2D(g_sampShadowMap, a_Input.projTex.xy + float2(0.0f, dx)).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s3 = (tex2D(g_sampShadowMap, a_Input.projTex.xy + float2(dx, dx)).r   + 0.00005f < depth) ? 0.0f : 1.0f;

	float shadowCoeff = 0.0f;
	
	shadowCoeff = lerp( lerp( s0, s1, lerps.x ), lerp( s2, s3, lerps.x ), lerps.y );

	return (tex2D(TexSamplerWrap, a_Input.tex) * shadowCoeff);
}

// vertex input structure used by both techniques
struct VSInput
{
	float3 pos: POSITION0;
	float3 norm: NORMAL0;
};

// vertex output structure used by ShadowMap technique
struct VSOutput
{
	float4 pos: POSITION0;
	float4 col: COLOR0;
	float4 projTex: TEXCOORD0;
};

// vertex output structure used by BuildShadowMap technique
struct VSOutput2
{
	float4 pos: POSITION0;
	float2 depth: TEXCOORD0;
};

VSOutput2 GenerateShadowMapVS(VSInput a_Input)
{
	VSOutput2 Output;

	// render position from light's perspective
	Output.pos = mul(float4(a_Input.pos, 1.0f), g_matLightWVP); //light WVP
	Output.depth = Output.pos.zw;

	return Output;
}

float4 GenerateShadowMapPS(VSOutput2 a_Input) : COLOR
{
	// output the depth of the pixel from the light source, normalized into the view space
	return a_Input.depth.x / a_Input.depth.y;
}

VSOutput ShadowMapVS(VSInput a_Input)
{
	VSOutput Output;

	// calculate vertex position homogenous
	Output.pos = mul(float4(a_Input.pos, 1.0f), g_matWorldViewProjection);

	// calculate vertex normal
	float3 normal = normalize(mul(float4(a_Input.norm, 0.0f), g_matWorldInverseTranspose).xyz);

	// calculate diffuse variable
	float diffComp = max(dot(g_vecLightDirection, normal), 0.0f);

	// calculate the two components
	float3 diffuse = diffComp * (g_vecLightDiffuse * g_vecMaterialDiffuse).rgb;
	float3 ambient = g_vecLightAmbient * g_vecMaterialAmbient;

	// combine and output colour
	Output.col = float4(diffuse + ambient, g_vecMaterialAmbient.a);

	Output.projTex = mul(float4(a_Input.pos, 1.0f), g_matLightWVP);

	return Output;
}

float4 ShadowMapPS(VSOutput a_Input) : COLOR
{
	// transform coordinates into texture coordinates
	a_Input.projTex.xy /= a_Input.projTex.w;            
	a_Input.projTex.x =  0.5f*a_Input.projTex.x + 0.5f; 
	a_Input.projTex.y = -0.5f*a_Input.projTex.y + 0.5f;
	
	// Compute pixel depth for shadowing.
	float depth = a_Input.projTex.z / a_Input.projTex.w;
 
	// Transform to texel space
    float2 texelpos = 512.0f * a_Input.projTex.xy;
        
    // Determine the lerp amounts.           
    float2 lerps = frac( texelpos );
    
    // sample shadow map
    float dx = 1.0f / 512.0f;
	float s0 = (tex2D(g_sampShadowMap, a_Input.projTex.xy).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s1 = (tex2D(g_sampShadowMap, a_Input.projTex.xy + float2(dx, 0.0f)).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s2 = (tex2D(g_sampShadowMap, a_Input.projTex.xy + float2(0.0f, dx)).r + 0.00005f < depth) ? 0.0f : 1.0f;
	float s3 = (tex2D(g_sampShadowMap, a_Input.projTex.xy + float2(dx, dx)).r   + 0.00005f < depth) ? 0.0f : 1.0f;

	float shadowCoeff = 0.0f;
	
	shadowCoeff = lerp( lerp( s0, s1, lerps.x ), lerp( s2, s3, lerps.x ), lerps.y );

	// output colour multipled by shadow value
	return float4(a_Input.col.rgb * shadowCoeff, g_vecMaterialDiffuse.a);
}

//-------- Techniques ----------

technique PhongNormalTerrain
{
	pass P0
	{
		vertexShader = compile vs_3_0 VS_PhongTerrain();
		pixelShader = compile ps_3_0 PS_PhongNormalTerrain();
	}
}

technique BillboardingTech
{
	pass P0
	{
		vertexShader =	compile vs_2_0 VS_Texture();
		pixelShader =	compile ps_2_0 PS_Texture();

		AlphaTestEnable				= true;
		AlphaRef					= 0x000000AA;
		AlphaFunc					= GreaterEqual;
	}
}

// technique used to generate the shadow map
technique GenerateShadowMapTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 GenerateShadowMapVS();
        pixelShader  = compile ps_2_0 GenerateShadowMapPS();
	}
}

// technique used to render the scene using the shadow map
technique ShadowMapTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 ShadowMapVS();
		pixelShader = compile ps_2_0 ShadowMapPS();
	}
}