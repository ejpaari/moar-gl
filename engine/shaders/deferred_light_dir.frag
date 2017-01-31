layout(location = 0) out vec3 outColor;

layout (location = 12) uniform vec3 cameraPos;
layout (location = 13) uniform vec4 lightColor;
layout (location = 14) uniform vec3 lightPos;
layout (location = 15) uniform vec3 lightForward;
layout (location = 24) uniform sampler2D depthTex;
layout (location = 31) uniform sampler2D colorTex;
layout (location = 32) uniform sampler2D normalTex;
layout (location = 33) uniform sampler2D positionTex;
layout (location = 41) uniform vec2 screenSize;
layout (location = 42) uniform int shadowsEnabled;
layout (location = 43) uniform float farPlane;
layout (location = 50) uniform mat4 lightSpaceProj;

in vec2 texCoord;

void main()
{
  outColor = vec3(0.0);
  vec3 vertexPos = texture(positionTex, texCoord).xyz;

  float lightPower = lightColor.w;
  vec4 pos_Light = lightSpaceProj * vec4(vertexPos, 1.0);

  vec4 texColor = texture(colorTex, texCoord);
  vec3 normal = texture(normalTex, texCoord).rgb;
  vec3 lightDir = -lightForward;
  float diff = getDiffuse(normal, lightDir);

  float specular = getSpecular(normalize(cameraPos - vertexPos), lightDir, normal, texColor.a);
  vec3 specularComponent = vec3(specular * lightPower);

  float shadow = 1.0;
  if (shadowsEnabled > 0) {
    shadow = calcDirShadow(depthTex, pos_Light);
  }
  
  outColor +=
    lightColor.xyz * diff * lightPower * texColor.rgb +
    specularComponent;

  outColor.rgb *= shadow;
}
