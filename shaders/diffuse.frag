#version 450 core

in vec3 normal_Cam;
in vec3 vertexPos_World;
in vec3 lightDir_Cam;
in vec2 texCoord;

out vec4 outColor;

uniform sampler2D DiffuseTex;
uniform vec3 lightPos;
//uniform vec3 lightColor;
uniform float lightPower;

layout (std140) uniform LightBlock {
    vec3 lightColor;
};

void main()
{    
    vec3 n = normalize(normal_Cam);
    vec3 l = normalize(lightDir_Cam);
    float diff = clamp(dot(n,l), 0, 1);

    float lightDistance = length(lightPos - vertexPos_World);
    outColor = vec4(lightColor * lightPower * diff / (lightDistance * lightDistance), 1.0) * texture(DiffuseTex, texCoord);
}
