#version 450 core

in vec3 Color;
//in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D myTexture;

void main() {
//     outColor = vec4(Color, 1.0) * texture(myTexture, Texcoord);
    outColor = vec4(Color, 1.0);
}
