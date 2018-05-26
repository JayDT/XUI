/////////////
// DEFINES //
/////////////

#define MAX_LIGHTS                                      4

// Material types
#define EMT_SOLID                                       0
#define EMT_SOLID_2_LAYER                               1
#define EMT_LIGHTMAP                                    2
#define EMT_LIGHTMAP_ADD                                3
#define EMT_LIGHTMAP_M2                                 4
#define EMT_LIGHTMAP_M4                                 5
#define EMT_LIGHTMAP_LIGHTING                           6
#define EMT_LIGHTMAP_LIGHTING_M2                        7
#define EMT_LIGHTMAP_LIGHTING_M4                        8
#define EMT_DETAIL_MAP                                  9
#define EMT_SPHERE_MAP                                  10
#define EMT_REFLECTION_2_LAYER                          11
#define EMT_TRANSPARENT_ADD_COLOR                       12
#define EMT_TRANSPARENT_ALPHA_CHANNEL                   13
#define EMT_TRANSPARENT_ALPHA_CHANNEL_REF               14
#define EMT_TRANSPARENT_VERTEX_ALPHA                    15
#define EMT_TRANSPARENT_REFLECTION_2_LAYER              16

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    row_major matrix worldMatrix;
    row_major matrix viewMatrix;
    row_major matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////

struct VS_INPUT
{
    float3 Position  : POSITION0;
    float3 Normal    : NORMAL0;
    float4 color     : COLOR0;
    float2 texCoord  : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
    float4 color    : COLOR0;
    float3 normal   : NORMAL0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PS_INPUT vs_main(VS_INPUT input)
{
    //float4 position = float4(input.Position, 1.0f);

    float4 position = mul(float4(input.Position, 1.0f), worldMatrix);
    position = mul(position, mul(viewMatrix, projectionMatrix));
    //position = mul(position, worldMatrix);
    //position = float4(mul((float3)position, (float3x3)viewMatrix), 1);
    //position += float4(viewMatrix[3].xyz, 0);
    //position = mul(position, projectionMatrix);

    PS_INPUT output = (PS_INPUT)0;
    output.position = position;
    output.tex = input.texCoord;
    output.normal = mul(input.Normal, (float3x3)worldMatrix);
    output.color = input.color;
    
    return output;
}


/////////////
// GLOBALS //
/////////////

struct Light
{
    float4 Position;
    float4 Atten;
    float4 Diffuse;
    float4 Specular;
    float4 Ambient;
};

struct Material
{
    float4    Ambient;
    float4    Diffuse;
    float4    Specular;
    float4    Emissive;
    float     Shininess;
    int       Type;
};

cbuffer PixelConstats : register(b0)
{
    int         g_nTexture;
    int         g_bAlphaTest;
    int         g_iLightCount; // Number of lights enabled
    float       g_fAlphaRef;
    Material    g_material;    
    Light       g_lights[MAX_LIGHTS];
};

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct PixelOutput
{
    float4 FragColor    : SV_TARGET0;
    //float4 Normal     : SV_TARGET1;
    //float4 Depth     : SV_TARGET2;
    //float  FragDepth : DEPTH0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PixelOutput ps_main(PS_INPUT input)
{
    float4 textureColor;
    if (g_nTexture > 0)
    {
        textureColor = shaderTexture.Sample(SampleType, input.tex).bgra;

        // check material type, and add color operation correctly
        [branch]
        switch(g_material.Type)
        {
        default:
            textureColor = float4(1.0, 1.0, 1.0, 1.0);
            break;
        case EMT_SOLID:
            textureColor = (textureColor * input.color.bgra);
            break;
        case EMT_SOLID_2_LAYER:
            textureColor = (textureColor * input.color.bgra) * g_material.Ambient;
            break;
        case EMT_TRANSPARENT_ALPHA_CHANNEL:
        case EMT_TRANSPARENT_ADD_COLOR:	// TODO
            textureColor = (textureColor * input.color.bgra);
            break;
        case EMT_TRANSPARENT_VERTEX_ALPHA:
            textureColor = (textureColor * input.color.bgra);
            break;
        };
        
        [branch]
        if (g_bAlphaTest > 0)
        {
            clip(textureColor.a - g_fAlphaRef);
        }
    }
    else
    {
        switch (g_material.Type)
        {
            case EMT_SOLID:
            default:
                textureColor = input.color.bgra;
                break;
            case EMT_SOLID_2_LAYER:
                textureColor = (g_material.Ambient * input.color.bgra);
                break;
        };
    }
    
    PixelOutput pixelData = (PixelOutput)0;
    pixelData.FragColor = saturate(textureColor);

    return pixelData;
}
