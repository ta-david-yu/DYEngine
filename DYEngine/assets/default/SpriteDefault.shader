@Blend SrcAlpha OneMinusSrcAlpha
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

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
    v_Color = color;
    v_TexCoord = texCoord;

    vec4 point4 = vec4(position, 1.0);
    gl_Position = _ProjectionMatrix * _ViewMatrix * _ModelMatrix * point4;
};

// Fragment shader
#Shader Fragment
#version 330 core

in vec4 v_Color;
in vec2 v_TexCoord;

@Property _MainTex "Main Texture"
uniform sampler2D _MainTex;

@Property _MainTex_TilingOffset "Main Texture | Tiling Offset"
uniform vec4 _MainTex_TilingOffset = vec4(1, 1, 0, 0);

@Property _Color "Color"
uniform vec4 _Color;

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

void main()
{
    // TilingOffset: xy -> tiling, zw -> offset
    vec2 tileOffsetTexCoord = _MainTex_TilingOffset.zw + _MainTex_TilingOffset.xy * v_TexCoord;
    vec4 finalColor = v_Color * _Color * texture(_MainTex, tileOffsetTexCoord);

    if (finalColor.a < 0.01)
    {
        discard;
    }

    color = finalColor;
    color2 = 50;
};