#version 450 core

in vec3 Color;
in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D myTexture;

void main()
{
    outColor = vec4(Color, 1.0) * texture(myTexture, TexCoord);
    //outColor = texture(myTexture, TexCoord);
    //outColor = vec4(Color, 1.0);
}
