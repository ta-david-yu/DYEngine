@Blend Off
@ZWrite Off
@ZTest Less

// Vertex shader
#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

uniform mat4 _ModelMatrix;
uniform mat4 _ViewMatrix;
uniform mat4 _ProjectionMatrix;


void main()
{
    vec4 point4 = vec4(position, 1.0);
    gl_Position = _ProjectionMatrix * _ViewMatrix * _ModelMatrix * point4;
};

// Fragment shader
#Shader Fragment
#version 330 core

layout(location = 0) out int entityId;

uniform int _EntityID;

void main()
{
    entityId = _EntityID;
};