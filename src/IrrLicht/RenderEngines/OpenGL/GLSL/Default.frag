#version 430 core

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

//////////////
// TYPEDEFS //
//////////////

in vertex_output
{
    vec4 position;
    vec2 tex;
    vec4 color;
    vec3 normal;
} _input;

out vec4 FragColor;

/////////////
// GLOBALS //
/////////////

layout(std430) struct Light
{
    vec3 Position;
    vec3 Atten;
    vec4 Diffuse;
    vec4 Specular;
    vec4 Ambient;
};

layout(std430) struct Material
{
    vec4    Ambient;
    vec4    Diffuse;
    vec4    Specular;
    vec4    Emissive;
    float   Shininess;
    int     Type;
};

layout(std430, binding = 2) buffer PixelConstats
{
    int         g_nTexture;
    int         g_bAlphaTest;
    int         g_iLightCount; // Number of lights enabled
    float       g_fAlphaRef;
    Material    g_material;    
    Light       g_lights[MAX_LIGHTS];
};

layout(location=0, binding=0) uniform sampler2D shaderTexture;

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
void main()
{
    vec4 textureColor;
    if (g_nTexture > 0)
    {
        textureColor = texture(shaderTexture, _input.tex);

        switch(g_material.Type)
        {
        default:
            textureColor = vec4(1.0, 1.0, 1.0, 1.0);
            break;
        case EMT_SOLID:
            textureColor = (textureColor * _input.color);
            break;
        case EMT_SOLID_2_LAYER:
            textureColor = (textureColor * _input.color) * g_material.Ambient;
            break;
        case EMT_TRANSPARENT_ALPHA_CHANNEL:
        case EMT_TRANSPARENT_ADD_COLOR:	// TODO
            textureColor = (textureColor * _input.color);
            break;
        case EMT_TRANSPARENT_VERTEX_ALPHA:
            textureColor = (textureColor * _input.color);
            break;
        };

        if (g_bAlphaTest != 0 && g_fAlphaRef > textureColor.a)
            discard;
        
        //[branch]
        //if (g_bAlphaTest > 0)
        //{
        //    clip(textureColor.a - g_fAlphaRef);
        //}
    }
    else
    {
        switch (g_material.Type)
        {
            case EMT_SOLID:
            default:
                textureColor = _input.color;
                break;
            case EMT_SOLID_2_LAYER:
                textureColor = (g_material.Ambient * _input.color);
                break;
        };
    }
    
    FragColor = textureColor;
}
