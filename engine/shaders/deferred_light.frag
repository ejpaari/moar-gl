layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outBloom;

layout (location = 12) uniform vec3 cameraPos;
layout (location = 23) uniform samplerCube depthTex;
layout (location = 30) uniform sampler2D colorTex;
layout (location = 31) uniform sampler2D normalTex;
layout (location = 32) uniform sampler2D positionTex;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
  vec4 lightColor;
  vec3 lightPos;
  vec3 lightForward;
};

in vec2 texCoord;

void main()
{
  outColor = vec3(0.0);
  vec3 vertexPos = texture(positionTex, texCoord).xyz;
  float lightDistance = length(lightPos - vertexPos);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;

  vec4 texColor = texture(colorTex, texCoord);
  vec3 normal = texture(normalTex, texCoord).rgb;
  vec3 lightDir = normalize(lightPos - vertexPos);
  float diff = clamp(dot(normal, lightDir), 0, 1);

  vec3 e = normalize(cameraPos - vertexPos);
  vec3 r = reflect(-lightDir, normal);
  float spec = clamp(dot(e, r), 0, 1);
  float specular = pow(spec, 10.0f) * texColor.a;

  float shadow = 1.0;
  bool receiveShadows = texture(normalTex, texCoord).a > 0.0;
  if (receiveShadows) {
    shadow = calcPointShadow(depthTex, vertexPos, lightPos, farPlane);
  }
  
  outColor +=
    (lightColor.xyz * diff * lightPower) * texColor.rgb +
    vec3(specular * lightPower / lightDistSqr);

  outColor.rgb *= shadow;

  float bloom = dot(outColor.rgb, vec3(1.0));
  outBloom = vec3(0.0);
  if (bloom > 2.0) {
    outBloom.rgb = outColor.rgb;
  }
}
