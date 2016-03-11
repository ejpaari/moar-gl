in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout (location = 10) uniform vec3 ambient;
layout (location = 20) uniform sampler2D diffuseTex;

void main()
{
    vec4 texColor = texture(diffuseTex, texCoord);
    if (texColor.a < 0.1) {
        discard;
    }
    outColor = vec4(ambient, 1.0) * texColor;
}
