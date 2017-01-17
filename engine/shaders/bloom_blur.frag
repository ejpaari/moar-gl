layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D bloomImage;
layout (location = 44) uniform bool horizontal;

in vec2 texCoord;

float weights[5] = float[](0.227, 0.1946, 0.12162, 0.0540, 0.01621);

void main()
{
  outColor = texture(bloomImage, texCoord).rgb * weights[0];
  vec2 offset = 1.0 / textureSize(bloomImage, 0);
  if (horizontal) {
    for (int i = 1; i < 5; ++i) {
      outColor += texture(bloomImage, texCoord + vec2(offset.x * i, 0.0)).rgb * weights[i];
      outColor += texture(bloomImage, texCoord - vec2(offset.x * i, 0.0)).rgb * weights[i];
    }
  } else {
    for (int i = 1; i < 5; ++i) {
      outColor += texture(bloomImage, texCoord + vec2(0.0, offset.y * i)).rgb * weights[i];
      outColor += texture(bloomImage, texCoord - vec2(0.0, offset.y * i)).rgb * weights[i];
    }
  }
}
