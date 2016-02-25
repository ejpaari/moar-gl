#version 450 core

in vec3 lightDir_Tan;
in vec2 texCoord;
in vec3 N;
in vec3 T;
in vec3 B;
in vec3 eyeDir_Cam;
in vec4 pos_Light;

layout(location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 21) uniform sampler2D normalTex;
layout (location = 22) uniform sampler2D bumpTex;
layout (location = 23) uniform sampler2D depthTex;
layout (location = 42) uniform int receiveShadows;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

#moar::include "../moar-gl/shaders/shadow_dir.glsl"

const float BUMP_DEPTH = 0.025;
const int NUM_STEPS = 50;

void main()
{
    vec3 dir = -eyeDir_Cam;
    vec2 step = vec2(dot(dir, normalize(T)), dot(dir, normalize(B)));
    float stepDepth = BUMP_DEPTH / NUM_STEPS;
    float currentDepth = 0.0;
    float height = texture(bumpTex, texCoord + currentDepth * step).r * BUMP_DEPTH;

    while ((BUMP_DEPTH - currentDepth) > height) {
        height = texture(bumpTex, texCoord + currentDepth * step).r * BUMP_DEPTH;
        currentDepth += stepDepth;
    }
    
    vec2 bumpTexCoord = texCoord + currentDepth * step;

    vec3 normal_Tan = normalize(texture(normalTex, bumpTexCoord).rgb * 2.0 - vec3(1.0));
    float diff = clamp(dot(normal_Tan, lightDir_Tan), 0, 1);
        float shadow = receiveShadows != 0 ? calcShadow(depthTex, pos_Light) : 1.0;

    outColor = shadow * vec4(lightColor.xyz * lightColor.w * diff, 1.0) * texture(diffuseTex, bumpTexCoord);
}
