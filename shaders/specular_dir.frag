#version 450 core

in vec3 normal_Cam;
in vec3 vertexPos_World;
in vec3 lightDir_Cam;
in vec2 texCoord;
in vec3 eyeDir_Cam;

layout(location = 0) out vec4 outColor;

layout (location = 11) uniform float Specularity;
layout (location = 20) uniform sampler2D DiffuseTex;

layout (std140) uniform LightBlock {
    vec4 lightColor;
    vec3 lightPos;
    vec3 lightForward;
};

void main()
{    
    vec3 n = normalize(normal_Cam);
    vec3 l = normalize(lightDir_Cam);
    float diff = clamp(dot(n,l), 0, 1);

    vec3 e = normalize(eyeDir_Cam);
    vec3 r = reflect(-l,n);
    float spec = clamp(dot(e,r), 0, 1);

    float lightDistance = length(lightPos - vertexPos_World);
    vec4 diffuseColor = texture(DiffuseTex, texCoord);
    float power = lightColor.w;
    float specular = pow(spec, Specularity);
    outColor = 
        vec4(lightColor.xyz * power * diff / (lightDistance * lightDistance), 1.0) * diffuseColor +
        vec4(vec3(1.0, 1.0, 1.0) * power * specular / (lightDistance * lightDistance), 1.0);
}
