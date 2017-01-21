in vec4 fragPos;

layout (location = 13) uniform vec4 lightColor;
layout (location = 14) uniform vec3 lightPos;
layout (location = 15) uniform vec3 lightForward;
layout (location = 43) uniform float farPlane;

void main()
{
    gl_FragDepth = length(fragPos.xyz - lightPos) / farPlane;
}  
