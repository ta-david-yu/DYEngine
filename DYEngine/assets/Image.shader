#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
uniform mat4 _TransformMatrix;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = texCoord;

    vec4 point4 = vec4(position, 0.0, 1.0);
    gl_Position = _TransformMatrix * point4;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
uniform vec4 _Color;

in vec2 v_TexCoord;

void main()
{
    color = _Color * vec4(v_TexCoord, 0.0, 1.0);
};