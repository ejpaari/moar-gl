in vec2 texCoord;

out vec4 outColor;

layout (location = 30) uniform sampler2D bloomImage;
layout (location = 44) uniform bool horizontal;

float weights[5] = float[](0.227, 0.195, 0.122, 0.054, 0.016);

void main()
{             
  vec2 offset = 1.0 / textureSize(bloomImage, 0);
  vec3 result = texture(bloomImage, texCoord).rgb * weights[0];
  if (result != vec3(0.0)) {
    if (horizontal) {
      for(int i = 1; i < 5; ++i) {
        result += texture(bloomImage, texCoord + vec2(offset.x * i, 0.0)).rgb * weights[i];
        result += texture(bloomImage, texCoord - vec2(offset.x * i, 0.0)).rgb * weights[i];
      }
    } else {
      for(int i = 1; i < 5; ++i) {
        result += texture(bloomImage, texCoord + vec2(0.0, offset.y * i)).rgb * weights[i];
        result += texture(bloomImage, texCoord - vec2(0.0, offset.y * i)).rgb * weights[i];
      }
    }
  }
  outColor = vec4(result, 1.0);
}
