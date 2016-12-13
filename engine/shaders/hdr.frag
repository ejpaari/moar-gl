layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D renderedTex;

in vec2 texCoord;

void main()
{
    const float gamma = 2.2;
    const float exposure = 1.5;
    vec3 hdrColor = texture(renderedTex, texCoord).rgb;      
    outColor = vec3(1.0) - exp(-hdrColor * exposure);
    //vec3 hdr = hdrColor / (hdrColor + vec3(1.0));
    //hdr = pow(hdr, vec3(1.0 / gamma));
}
