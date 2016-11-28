in vec2 texCoord;

layout(location = 0) out vec4 outColor0;
layout(location = 1) out vec4 outColor1;

layout (location = 10) uniform vec3 ambient;
layout (location = 20) uniform sampler2D diffuseTex;

void main()
{
    vec4 texColor = texture(diffuseTex, texCoord);
    if (texColor.a < 0.1) {
        discard;
    }
    outColor0 = vec4(ambient, 1.0) * texColor;
    outColor1 = vec4(0.0);
}
