// References HLSL
// Flow control: http://msdn.microsoft.com/en-us/library/bb509600(VS.85).aspx
// Semantics: http://msdn.microsoft.com/en-us/library/bb509647(VS.85).aspx

// Constants
#define MAX_LIGHTS 									4
#define MAX_TEXTURES 								2
#define TEX1 										0
#define TEX2 										1
#define TEX3 										2
#define TEX4 										3
#define FOGMODE_NONE    							0
#define FOGMODE_LINEAR  							1
#define FOGMODE_EXP     							2
#define FOGMODE_EXP2    							3
#define E 									  2.71828
#define USE_ALPHA_TEST 								1
#define USE_CLIPPLANES 								0	// disable clip planes by now
#define USE_FOG 									0	// disable fog by now

// Material types									
#define EMT_SOLID									0
#define EMT_SOLID_2_LAYER							1
#define EMT_LIGHTMAP								2
#define EMT_LIGHTMAP_ADD							3
#define EMT_LIGHTMAP_M2								4
#define EMT_LIGHTMAP_M4								5
#define EMT_LIGHTMAP_LIGHTING						6
#define EMT_LIGHTMAP_LIGHTING_M2					7
#define EMT_LIGHTMAP_LIGHTING_M4					8
#define EMT_DETAIL_MAP								9
#define EMT_SPHERE_MAP								10
#define EMT_REFLECTION_2_LAYER						11
#define EMT_TRANSPARENT_ADD_COLOR					12
#define EMT_TRANSPARENT_ALPHA_CHANNEL				13
#define EMT_TRANSPARENT_ALPHA_CHANNEL_REF			14
#define EMT_TRANSPARENT_VERTEX_ALPHA				15
#define EMT_TRANSPARENT_REFLECTION_2_LAYER			16

// Normal mapping
#define EMT_NORMAL_MAP_SOLID						17
#define EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR		18
#define EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA		19

// Parllax mapping
#define EMT_PARALLAX_MAP_SOLID						20
#define EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR		21
#define EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA	22

#define EMT_TEXTURE_SPLAT							23
#define EMT_TEXTURE_SPLAT_2							24

// adding needed structs
struct ColorsOutput
{
    float4 Diffuse;
    float4 Specular;
};

struct Light
{
    float4 Position;
    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
    float4 Atten;
};

struct Material
{
	float4	Ambient;
	float4	Diffuse;
	float4	Specular;
	float4	Emissive;
	float	Shininess;
	int		Type;
};

struct VS_INPUT
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 color	: COLOR;
	float2 tex0		: TEXCOORD0;
};

struct VS_INPUT_2COORDS
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 color	: COLOR;
	float2 tex0		: TEXCOORD0;
	float2 tex1		: TEXCOORD1;
};

struct VS_INPUT_TANGENTS
{
	float4 pos		: POSITION;
	float3 norm		: NORMAL;
	float4 color	: COLOR;
	float2 tex0		: TEXCOORD0;
	float3 tangent	: TEXCOORD1;
	float3 binormal : TEXCOORD2;
};

struct VS_OUTPUT
{
	float4 pos		: SV_Position;
	float2 tex0		: TEXTURE0;
	float3 wPos		: TEXTURE1;         //world space pos
	float3 wNorm	: TEXTURE2;        //world space normal
	float4 colorD	: COLOR0;
	float4 colorS	: COLOR1;
#if USE_FOG
    float  fogDist	: FOGDISTANCE;
#endif
#if USE_CLIPPLANES
	float3 planeDist : SV_ClipDistance0;
#endif
};

struct VS_OUTPUT_2COORDS
{
	float4 pos		: SV_Position;
	float2 tex0		: TEXTURE0;
	float2 tex1		: TEXTURE1;
	float3 wPos		: TEXTURE2;         //world space pos
	float3 wNorm	: TEXTURE3;        //world space normal
	float4 colorD	: COLOR0;
	float4 colorS	: COLOR1;
#if USE_FOG
    float  fogDist	: FOGDISTANCE;
#endif
#if USE_CLIPPLANES
	float3 planeDist : SV_ClipDistance0;
#endif
};

struct VS_OUTPUT_TANGENTS
{
	float4 pos		: SV_Position;
	float2 tex0		: TEXTURE0;
	float3 tangent	: TANGENT0;
	float3 binormal : BINORMAL0;
	float3 wPos		: TEXTURE1;         //world space pos
	float3 wNorm	: TEXTURE2;        //world space normal
	float4 colorD	: COLOR0;
	float4 colorS	: COLOR1;
#if USE_FOG
    float  fogDist	: FOGDISTANCE;
#endif
#if USE_CLIPPLANES
	float3 planeDist : SV_ClipDistance0;
#endif
};

struct PS_INPUT
{
	float4 pos		: SV_Position;
	float2 tex0		: TEXTURE0;
	float3 wPos		: TEXTURE1;         //world space pos
	float3 wNorm	: TEXTURE2;        //world space normal
	float4 colorD	: COLOR0;
	float4 colorS	: COLOR1;
#if USE_FOG
    float  fogDist : FOGDISTANCE;
#endif
};

struct PS_INPUT_2COORDS
{
	float4 pos		: SV_Position;
	float2 tex0		: TEXTURE0;
	float2 tex1		: TEXTURE1;
	float3 wPos		: TEXTURE2;         //world space pos
	float3 wNorm	: TEXTURE3;        //world space normal
	float4 colorD	: COLOR0;
	float4 colorS	: COLOR1;
#if USE_FOG
    float  fogDist : FOGDISTANCE;
#endif
};

struct PS_INPUT_TANGENTS
{
	float4 pos		: SV_Position;
	float2 tex0		: TEXTURE0;
	float3 tangent	: TANGENT0;
	float3 binormal	: BINORMAL0;
	float3 wPos		: TEXTURE1;			//world space pos
	float3 wNorm	: TEXTURE2;			//world space normal
	float4 colorD	: COLOR0;
	float4 colorS	: COLOR1;
#if USE_FOG
    float  fogDist	: FOGDISTANCE;
#endif
};

// adding constant buffer for transform matrices
cbuffer cbPerFrame : register(c0)
{
    float4x4 g_mWorld;
    float4x4 g_mView;
    float4x4 g_mProj;
	float4x4 TextureMatrix0;
};

// adding constant buffer for fog, clip planes and point draw
cbuffer cbPerTechnique : register(c1)
{
    bool     g_bEnableLighting = false;
#if USE_CLIPPLANES
    bool     g_bEnableClipping = false;
#endif
    bool     g_bPointScaleEnable = false;
    float    g_pointScaleA;
    float    g_pointScaleB;
    float    g_pointScaleC;
    float    g_pointSize;   
#if USE_FOG
    int      g_fogMode = FOGMODE_NONE;
    float    g_fogStart;
    float    g_fogEnd;
    float    g_fogDensity;
    float4   g_fogColor;
#endif
#if USE_ALPHA_TEST
	bool	g_bAlphaTest = false;
	float	g_fAlphaRef = 0.5f;
#endif
};

// adding constant buffer for lightning information
cbuffer cbLights : register(c2)
{
	int			g_iLightCount=0; // Number of lights enabled
#if USE_CLIPPLANES
	float4		g_clipplanes[3];
#endif
	Light		g_lights[MAX_LIGHTS];
	Material	g_material;
};

// adding function to calculate lightning
ColorsOutput CalcLighting( float3 worldNormal, float3 worldPos, float3 cameraPos, float4 vertexColour)
{
   ColorsOutput output = (ColorsOutput)0.0;

	const int nLights = min(MAX_LIGHTS, g_iLightCount); // Clamp to MAX_LIGHTS
	for(int i=0; i<nLights; i++)
	{
		float3 toLight = g_lights[i].Position.xyz - worldPos;
		float lightDist = length( toLight );
		float fAtten = 1.0/dot( g_lights[i].Atten, float4(1,lightDist,lightDist*lightDist,0) );
		float3 lightDir = normalize( toLight );
		float3 halfAngle = normalize( normalize(-cameraPos) + lightDir );

		float4 _Ambient = g_lights[i].Ambient * g_material.Ambient;
		float4 _Diffuse = g_lights[i].Diffuse * g_material.Diffuse * vertexColour;
		float4 _Specular = g_lights[i].Specular * g_material.Specular;
		float4 _Emissive = g_material.Emissive;

		output.Diffuse += max(0,dot( lightDir, worldNormal ) * _Diffuse * fAtten) + _Ambient + _Emissive;
		output.Specular += max(0,pow( dot( halfAngle, worldNormal ), 64 ) * _Specular * fAtten );
	}

	return output;
}

// adding function to calculate fog
#if USE_FOG
float CalcFogFactor( float d )
{
    float fogCoeff = 1.0;
    
	[branch]
	switch( g_fogMode )
	{
	case FOGMODE_LINEAR:
		fogCoeff = (g_fogEnd - d)/(g_fogEnd - g_fogStart);
		break;
	case FOGMODE_EXP:
		fogCoeff = 1.0 / pow( E, d*g_fogDensity );
		break;
	case FOGMODE_EXP2:
		fogCoeff = 1.0 / pow( E, d*d*g_fogDensity*g_fogDensity );
		break;
	};
		
    return clamp( fogCoeff, 0, 1 );
}
#endif
	
// adding vertex shader code
VS_OUTPUT StandardVS( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	//output our final position in clipspace
	float4 worldPos = mul( input.pos, g_mWorld );
	float4 cameraPos = mul( worldPos, g_mView ); //Save cameraPos for fog calculations
	output.pos = mul( cameraPos, g_mProj );

	output.wPos = worldPos;
	//save the fog distance for later
#if USE_FOG
	//output.fogDist = cameraPos.z;
	output.fogDist = distance(output.pos, cameraPos);
#endif

#if USE_CLIPPLANES
    //find our clipping planes (fixed function clipping is done in world space)
	[branch]
    if( g_bEnableClipping )
    {
        worldPos.w = 1;
        
        //calc the distance from the 3 clipping planes
        output.planeDist.x = dot( worldPos, g_clipplanes[0] );
        output.planeDist.y = dot( worldPos, g_clipplanes[1] );
        output.planeDist.z = dot( worldPos, g_clipplanes[2] );
    }
	else
    {
        output.planeDist.x = 1;
        output.planeDist.y = 1;
        output.planeDist.z = 1;
    }
#endif

	float4 diffuseSwizzle = input.color.bgra; // swizzle the input rgba channels into D3D11 order
	[branch]
	if( g_bEnableLighting )
	{
		float3 worldNormal = normalize( mul( input.norm, (float3x3)g_mWorld ) );
		output.wNorm = worldNormal;
		ColorsOutput cOut = CalcLighting( worldNormal, worldPos, cameraPos, diffuseSwizzle );
		output.colorD = cOut.Diffuse;
		output.colorS = cOut.Specular;
	}
	else
	{ 
		output.colorD = diffuseSwizzle;
	}

	// propagate texture coordinate
	output.tex0 = input.tex0;
	return output;
}

// adding vertex shader code
VS_OUTPUT_2COORDS Coords2TVS( VS_INPUT_2COORDS input )
{
	VS_OUTPUT_2COORDS output = (VS_OUTPUT_2COORDS)0;

	//output our final position in clipspace
	float4 worldPos = mul( input.pos, g_mWorld );
	float4 cameraPos = mul( worldPos, g_mView ); //Save cameraPos for fog calculations
	output.pos = mul( cameraPos, g_mProj );

	output.wPos = worldPos;
	//save the fog distance for later
#if USE_FOG
	//output.fogDist = cameraPos.z;
	output.fogDist = distance(output.pos, cameraPos);
#endif

#if USE_CLIPPLANES
    //find our clipping planes (fixed function clipping is done in world space)
	[branch]
    if( g_bEnableClipping )
    {
        worldPos.w = 1;
        
        //calc the distance from the 3 clipping planes
        output.planeDist.x = dot( worldPos, g_clipplanes[0] );
        output.planeDist.y = dot( worldPos, g_clipplanes[1] );
        output.planeDist.z = dot( worldPos, g_clipplanes[2] );
    }
	else
    {
        output.planeDist.x = 1;
        output.planeDist.y = 1;
        output.planeDist.z = 1;
    }
#endif

	float4 diffuseSwizzle = input.color.bgra; // swizzle the input rgba channels into D3D10 order
	[branch]
	if( g_bEnableLighting )
	{
		float3 worldNormal = normalize( mul( input.norm, (float3x3)g_mWorld ) );
		output.wNorm = worldNormal;
		ColorsOutput cOut = CalcLighting( worldNormal, worldPos, cameraPos, diffuseSwizzle );
		output.colorD = cOut.Diffuse;
		output.colorS = cOut.Specular;
	}
	else
	{ 
		output.colorD = diffuseSwizzle;
	}

	// propagate texture coordinate
	output.tex0 = input.tex0;
	output.tex1 = input.tex1;
	return output;
}

// adding vertex shader code
VS_OUTPUT_TANGENTS TangentsVS( VS_INPUT_TANGENTS input )
{
	VS_OUTPUT_TANGENTS output = (VS_OUTPUT_TANGENTS)0;

	//output our final position in clipspace
	float4 worldPos = mul( input.pos, g_mWorld );
	float4 cameraPos = mul( worldPos, g_mView ); //Save cameraPos for fog calculations
	output.pos = mul( cameraPos, g_mProj );

	output.wPos = worldPos;
	//save the fog distance for later
#if USE_FOG
	//output.fogDist = cameraPos.z;
	output.fogDist = distance(output.pos, cameraPos);
#endif

#if USE_CLIPPLANES
    //find our clipping planes (fixed function clipping is done in world space)
	[branch]
    if( g_bEnableClipping )
    {
        worldPos.w = 1;
        
        //calc the distance from the 3 clipping planes
        output.planeDist.x = dot( worldPos, g_clipplanes[0] );
        output.planeDist.y = dot( worldPos, g_clipplanes[1] );
        output.planeDist.z = dot( worldPos, g_clipplanes[2] );
    }
	else
    {
        output.planeDist.x = 1;
        output.planeDist.y = 1;
        output.planeDist.z = 1;
    }
#endif

	float4 diffuseSwizzle = input.color.bgra; // swizzle the input rgba channels into D3D10 order
	[branch]
	if( g_bEnableLighting )
	{
		float3 worldNormal = normalize( mul( input.norm, (float3x3)g_mWorld ) );
		output.wNorm = worldNormal;
		ColorsOutput cOut = CalcLighting( worldNormal, worldPos, cameraPos, diffuseSwizzle );
		output.colorD = cOut.Diffuse;
		output.colorS = cOut.Specular;
	}
	else
	{ 
		output.colorD = diffuseSwizzle;
	}

	// propagate texture coordinate
	output.tex0 = input.tex0;
	output.tangent = input.tangent;
	output.binormal = input.binormal;
	return output;
}

// adding textures and samplers
Texture2D g_tex1 : register(t0);
Texture2D g_tex2 : register(t1);
//Texture2D g_tex3 : register(t2);
//Texture2D g_tex4 : register(t3);

SamplerState g_sampler1 : register(s0);
SamplerState g_sampler2 : register(s1);
//SamplerState g_sampler3 : register(s2);
//SamplerState g_sampler4 : register(s3);

// adding pixel shader
float4 StandardPS( PS_INPUT input ) : SV_Target
{
	float4 normalColor = float4(0, 0, 0, 1);
	float4 tex1 = g_tex1.Sample( g_sampler1, input.tex0 ).bgra;

// add alpha test
#if USE_ALPHA_TEST
	[branch]
	if (g_bAlphaTest)
	{
		clip(tex1.a - g_fAlphaRef);
	}
#endif
	
	// check material type, and add color operation correctly
	[branch]
	switch(g_material.Type)
	{
	case EMT_SOLID:
		normalColor = (tex1 * input.colorD) + input.colorS;
		break;
	case EMT_TRANSPARENT_ADD_COLOR:	// TODO
		normalColor = (tex1 /** input.colorD*/) + input.colorS;
		break;
	case EMT_TRANSPARENT_VERTEX_ALPHA:
		//normalColor = tex1 + input.colorS;
		normalColor = (tex1 /** input.colorD*/) + input.colorS;
		//normalColor = (float4(tex1.rgb, input.colorD.a) /** input.colorD*/) + input.colorS;
		break;
	default:
		normalColor = float4(1,1,1,1);
		break;
	};

// add fog calculation
#if USE_FOG
	float fog = CalcFogFactor( input.fogDist );
	normalColor = fog * normalColor + (1.0 - fog)*g_fogColor;
#endif
							
	// return color in pixel shader
	return normalColor;
}

float4 Coords2TPS( PS_INPUT_2COORDS input ) : SV_Target
{
	float4 normalColor = float4(0, 0, 0, 1);
	float4 tex1 = g_tex1.Sample( g_sampler1, input.tex0 ).bgra;
	float4 tex2 = g_tex2.Sample( g_sampler2, input.tex1 ).bgra;

// add alpha test
#if USE_ALPHA_TEST
	[branch]
	if (g_bAlphaTest)
	{
		clip(tex1.a - g_fAlphaRef);
	}
#endif
	
	// check material type, and add color operation correctly
	[branch]
	switch(g_material.Type)
	{
	case EMT_SOLID:
		normalColor = (tex1 * input.colorD) + input.colorS;
		break;
	case EMT_SOLID_2_LAYER:
		normalColor = lerp(tex1, tex2, tex1.a) * input.colorD + input.colorS;
		break;
	case EMT_LIGHTMAP:
		normalColor = tex1 * tex2;
		break;
	case EMT_LIGHTMAP_ADD:
		normalColor = tex1 + tex2;
		break;
	case EMT_LIGHTMAP_M2:
		normalColor = (tex1 * tex2) * 2;
		break;
	case EMT_LIGHTMAP_M4:
		normalColor = (tex1 * tex2) * 4;
		break;
	case EMT_LIGHTMAP_LIGHTING:
		normalColor = tex1 * tex2;
		normalColor = (normalColor * input.colorD) + input.colorS;
		break;
	case EMT_LIGHTMAP_LIGHTING_M2:
		normalColor = (tex1 * tex2) * 2;
		normalColor = (normalColor * input.colorD) + input.colorS;
		break;
	case EMT_LIGHTMAP_LIGHTING_M4:
		normalColor = (tex1 * tex2) * 4;
		normalColor = (normalColor * input.colorD) + input.colorS;
		break;
	case EMT_DETAIL_MAP:
		normalColor = (tex1 + tex2) - 0.5;
		break;
	case EMT_SPHERE_MAP:			// TODO
	case EMT_REFLECTION_2_LAYER:	// TODO
	case EMT_TRANSPARENT_ADD_COLOR:	// TODO
		normalColor = (tex1 /** input.colorD*/) + input.colorS;
		break;
	case EMT_TRANSPARENT_ALPHA_CHANNEL:	// TODO
	case EMT_TRANSPARENT_ALPHA_CHANNEL_REF:	// TODO
	case EMT_TRANSPARENT_VERTEX_ALPHA:
		normalColor = (tex1 /** input.colorD*/) + input.colorS;
		//normalColor = (float4(tex1.rgb, input.colorD.a) /** input.colorD*/) + input.colorS;
		break;
	case EMT_TRANSPARENT_REFLECTION_2_LAYER:	// TODO
	default:
		normalColor = float4(1,1,1,1);
		break;
	}

// add fog calculation
#if USE_FOG
	float fog = CalcFogFactor( input.fogDist );
	normalColor = fog * normalColor + (1.0 - fog)*g_fogColor;
#endif
							
	// return color in pixel shader
	return normalColor;
}

float4 TangentsPS( PS_INPUT_TANGENTS input ) : SV_Target
{
	float4 normalColor = float4(0, 0, 0, 1);
	float4 tex1 = g_tex1.Sample( g_sampler1, input.tex0 ).bgra;

// add alpha test
#if USE_ALPHA_TEST
	[branch]
	if (g_bAlphaTest)
	{
		clip(tex1.a - g_fAlphaRef);
	}
#endif
	
	// Only use EMT_SOLID for now
	normalColor = (tex1 * input.colorD) + input.colorS;

// add fog calculation
#if USE_FOG
	float fog = CalcFogFactor( input.fogDist );
	normalColor = fog * normalColor + (1.0 - fog)*g_fogColor;
#endif
							
// return color in pixel shader
	return normalColor;
}

// Technique for standard vertex type
technique11 StandardTech
{
	pass p0
	{
		SetVertexShader( CompileShader( vs_4_0, StandardVS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, StandardPS() ) );
	}  
}

// Technique for 2 coords vertex type
technique11 Coords2TTech
{
	pass p0
	{
		SetVertexShader( CompileShader( vs_4_0, Coords2TVS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, Coords2TPS() ) );
	}  
}

// Technique for tangents vertex type
technique11 TangentsTech
{
	pass p0
	{
		SetVertexShader( CompileShader( vs_4_0, TangentsVS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, TangentsPS() ) );
	}  
}