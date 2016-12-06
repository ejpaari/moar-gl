layout(location = 0) out vec4 outColor;

layout (location = 30) uniform sampler2D diffuseTex;
layout (location = 31) uniform sampler2D normalTex;
layout (location = 32) uniform sampler2D positionTex;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

in vec2 texCoord;

void main()
{
  outColor = vec4(0.0);
  vec3 vertexPos_World = texture(positionTex, texCoord).xyz;
  float lightDistance = length(lightPos - vertexPos_World);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;

  vec4 texColor = texture(diffuseTex, texCoord);
  vec3 normal_World = texture(normalTex, texCoord).xyz;
  vec3 lightDir_World = normalize(lightPos - vertexPos_World);
  float diff = clamp(dot(normal_World, lightDir_World), 0, 1);
  
  outColor += vec4(lightColor.xyz * diff * lightPower, 1.0) * texColor;
}
