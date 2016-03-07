#version 450 core

in vec3 normal_Cam;
in vec3 vertexPos_World;
in vec3 lightDir_Cam;
in vec2 texCoord;

layout (location = 0) out vec4 outColor;

layout (location = 20) uniform sampler2D diffuseTex;
layout (location = 23) uniform samplerCube depthTex;
layout (location = 42) uniform int receiveShadows;
layout (location = 43) uniform float farPlane;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

#moar::include "../moar-gl/shaders/shadow_point.glsl"
#moar::include "../moar-gl/shaders/discard.glsl"

void main()
{    
    vec4 texColor = texture(diffuseTex, texCoord);
    float lightDistance = length(lightPos - vertexPos_World);
    float lightPower = lightColor.w / (lightDistance * lightDistance);

    if (isTransparent(texColor.a) || isTooFar(lightPower)) {
        discard;
    }

    vec3 n = normalize(normal_Cam);
    vec3 l = normalize(lightDir_Cam);
    float diff = clamp(dot(n,l), 0, 1);

    float shadow = receiveShadows != 0 ? 
          calcPointShadow(depthTex, vertexPos_World, lightPos, farPlane) : 
          1.0;

    outColor = shadow * 
               vec4(lightColor.xyz * diff * lightPower, 1.0) * 
               texColor;
}