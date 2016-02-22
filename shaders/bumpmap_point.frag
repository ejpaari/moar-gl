#version 450 core

in vec3 vertexPos_World;
in vec3 lightDir_Tan;
in vec2 texCoord;
in vec3 N;
in vec3 T;
in vec3 B;
in vec3 eyeDir_Cam;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
layout (location = 23) uniform samplerCube depthTex;
layout (location = 42) uniform int receiveShadows;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

#moar::include "../moar-gl/shaders/shadow_point.glsl"

const float BUMP_DEPTH = 0.3;
const int NUM_STEPS = 100;
const float STEP_DEPTH = 0.0025;

void main()
{
    float currentDepth = 0.0;
    vec3 dir = -eyeDir_Cam;
    vec3 step = vec3(dot(dir, normalize(T)), dot(dir, normalize(B)), dot(dir, N));
    vec2 offsetStep = step.xy;
    float height = 0.0;

    for (int i = 0; i < NUM_STEPS; ++i) {
        height = texture(bumpTex, texCoord + currentDepth * offsetStep).r * BUMP_DEPTH;
        if (currentDepth < height) {
            currentDepth += STEP_DEPTH;
        } else {
            break;
        }
    }
    vec2 bumpTexCoord = texCoord + currentDepth * offsetStep;

    vec3 normal_Tan = normalize(texture(normalTex, bumpTexCoord).rgb * 2.0 - vec3(1.0));

    float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);

    float lightDistance = length(lightPos - vertexPos_World);
    float shadow = receiveShadows != 0 ? 
          calcPointShadow(depthTex, vertexPos_World, lightPos, farPlane) : 
          1.0;

    outColor = shadow *
               vec4(lightColor.xyz * lightColor.w * diff / (lightDistance * lightDistance), 1.0) *
               texture(diffuseTex, bumpTexCoord);
}
