layout(location = 0) out vec4 outColor;

layout (location = 10) uniform vec3 ambient;
layout (location = 12) uniform vec3 cameraPos;
layout (location = 30) uniform sampler2D colorTex;
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
  // Everything is in _World coordinate system unless specified otherwise
  outColor = vec4(0.0, 0.0, 0.0, 1.0);
  vec3 vertexPos = texture(positionTex, texCoord).xyz;
  float lightDistance = length(lightPos - vertexPos);
  float lightDistSqr = lightDistance * lightDistance;
  float lightPower = lightColor.w / lightDistSqr;

  vec4 texColor = texture(colorTex, texCoord);
  vec3 normal = texture(normalTex, texCoord).xyz;
  vec3 lightDir = normalize(lightPos - vertexPos);
  float diff = clamp(dot(normal, lightDir), 0, 1);

  vec3 e = normalize(cameraPos - vertexPos);
  vec3 r = reflect(-lightDir, normal);
  float spec = clamp(dot(e, r), 0, 1);
  float specular = pow(spec, 10.0f) * texColor.a;
  
  outColor += 
    vec4(ambient * texColor.rgb, 0.0) +
    vec4((lightColor.xyz * diff * lightPower) * texColor.rgb, 0.0) +
    vec4(vec3(specular * lightPower / lightDistSqr), 0.0);
}
