#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoord;

uniform mat4 _TransformMatrix;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
    v_Color = color;
    v_TexCoord = texCoord;

    vec4 point4 = vec4(position, 1.0);
    gl_Position = _TransformMatrix * point4;
};

#shader fragment
#version 330 core

in vec4 v_Color;
in vec2 v_TexCoord;

uniform sampler2D _MainTex;
uniform vec4 _Color;

layout(location = 0) out vec4 color;

void main()
{
    color = v_Color * _Color * texture(_MainTex, v_TexCoord);

    // Use this to visualize texture coordinate
    // vec4(v_TexCoord, 0.0, 1.0);
};