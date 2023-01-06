@Blend SrcAlpha OneMinusSrcAlpha
@ZWrite Off
@ZTest Always

#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 _ViewMatrix;
uniform mat4 _ProjectionMatrix;

out vec4 v_Color;

void main()
{
    v_Color = color;

    vec4 point4 = vec4(position, 1.0);
    gl_Position = _ProjectionMatrix * _ViewMatrix * point4;
};

#Shader Fragment
#version 330 core

in vec4 v_Color;

layout(location = 0) out vec4 color;

void main()
{
    vec4 finalColor = v_Color;
    color = finalColor;
};