#version 450 core

/////////////
// GLOBALS //
/////////////

layout (std430, binding = 1) buffer MatrixBuffer
{
    mat4 worldMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoord;

out vertex_output
{
    layout(location = 0) vec4 position;
    layout(location = 1) vec2 tex;
    layout(location = 2) vec4 color;
    layout(location = 3) vec3 normal;
} VertOut;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main()
{
    mat4 MWorldMatrix = projectionMatrix * viewMatrix;
    vec4 position = worldMatrix * vec4(Position, 1.0f);
    vec4 NormalL  = vec4(Normal, 0.0);

    position = MWorldMatrix * position;

    gl_Position = position;
    VertOut.position = gl_Position;
    VertOut.tex      = texCoord;
    VertOut.normal   = (MWorldMatrix * NormalL).xyz;
    VertOut.color    = color;
}
