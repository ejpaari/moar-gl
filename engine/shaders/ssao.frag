layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D positionTex;
layout (location = 45) uniform mat4 projection;
// SSAO_KERNEL_SIZE is defined as constant in cpp
layout (location = 80) uniform vec3 kernel[SSAO_KERNEL_SIZE];

in vec2 texCoord;

const float RADIUS = 0.1;
const float BIAS = 0.001;

void main()
{
  vec3 pos = texture(positionTex, texCoord).xyz;
  float occlusion = 0.0;

  for (int i = 0; i < SSAO_KERNEL_SIZE; i++) {
    vec3 samplePos = pos + kernel[i];
    vec4 offset = vec4(samplePos, 1.0);
    offset = projection * offset;
    offset.xy /= offset.w;
    offset.xy = offset.xy * 0.5 + vec2(0.5);

    float sampleDepth = texture(positionTex, offset.xy).z;
     if (abs(pos.z - sampleDepth) < RADIUS) {
    //   occlusion += sampleDepth > samplePos.z + BIAS ? 1.0 : 0.0;
       occlusion += step(samplePos.z, sampleDepth);
    }

    // float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(pos.z - sampleDepth));
    // occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
  }

  occlusion = 1.0 - (occlusion / SSAO_KERNEL_SIZE);
  occlusion = smoothstep(0.0, 0.5, occlusion);
  outColor = vec3(occlusion);
}
