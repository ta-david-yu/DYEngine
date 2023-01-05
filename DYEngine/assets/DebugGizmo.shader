@Blend SrcAlpha OneMinusSrcAlpha
@ZWrite Off
@ZTest Always

#Shader Vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec4 v_Color;

void main()
{
    v_Color = vec4(1, 0, 0, 1); //color;

    vec4 point4 = vec4(position, 1.0);
    gl_Position = point4;
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