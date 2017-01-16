layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D renderedTex;
layout (location = 31) uniform sampler2D ssaoTex;

in vec2 texCoord;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTex, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) {
        for (int y = -2; y < 2; ++y) {
            vec2 offset = vec2(x, float(y)) * texelSize;
            result += texture(ssaoTex, texCoord + offset).r;
        }
    }
    outColor = texture(renderedTex, texCoord).rgb * (result / 16.0);    
}
