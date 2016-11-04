in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D RenderedTex;

void main()
{
    const float gamma = 2.2;
    const float exposure = 1.5;
    vec3 hdrColor = texture(RenderedTex, texCoord).rgb;
      
    vec3 hdr = vec3(1.0) - exp(-hdrColor * exposure);
    //vec3 hdr = hdrColor / (hdrColor + vec3(1.0));
    //hdr = pow(hdr, vec3(1.0 / gamma));
  
    outColor = vec4(hdr, 1.0);
}
