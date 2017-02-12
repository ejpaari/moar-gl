// Based on https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl

layout(location = 0) out vec3 outColor;

layout (location = 30) uniform sampler2D renderedTex;
layout (location = 41) uniform vec2 screenSize;

const vec3 TO_LUMA = vec3(0.299, 0.587, 0.114); // http://en.wikipedia.org/wiki/Grayscale
const float LUMA_BIAS = 0.01f;
const float LUMA_THRESHOLD = 0.4f;
const float MUL_REDUCE = 0.125f;
const float MIN_REDUCE = 0.0078125f;
const float MAX_SPAN = 8.0f;
const vec2 MAX_SPAN_VEC = vec2(MAX_SPAN, MAX_SPAN);

in vec2 texCoord;

void main()
{
  vec2 texelStep = vec2(1.0) / screenSize;

  vec3 rgbM = texture(renderedTex, texCoord).rgb;
  vec3 rgbNW = textureOffset(renderedTex, texCoord, ivec2(-1, 1)).rgb;
  vec3 rgbNE = textureOffset(renderedTex, texCoord, ivec2(1, 1)).rgb;
  vec3 rgbSW = textureOffset(renderedTex, texCoord, ivec2(-1, -1)).rgb;
  vec3 rgbSE = textureOffset(renderedTex, texCoord, ivec2(1, -1)).rgb;

  // RGB to luma.
  float lumaM = dot(rgbM, TO_LUMA);
  float lumaNW = dot(rgbNW, TO_LUMA);
  float lumaNE = dot(rgbNE, TO_LUMA);
  float lumaSW = dot(rgbSW, TO_LUMA);
  float lumaSE = dot(rgbSE, TO_LUMA);

  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  if ((lumaMax - lumaMin) < (lumaMax * LUMA_THRESHOLD + LUMA_BIAS)) {
    outColor = rgbM;
    return;
  }  
  
  // Sampling is done along the gradient.
  vec2 samplingDirection;	
  samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  samplingDirection.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
  // Sampling step distance depends on the luma: 
  // The brighter the sampled texels, the smaller the final sampling step direction.
  // Therefore brighter areas are less blurred than dark areas.  
  float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * MUL_REDUCE, MIN_REDUCE);

  // Factor for norming the sampling direction plus adding the brightness influence. 
  float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
  // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size.
  samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, -MAX_SPAN_VEC, MAX_SPAN_VEC) * texelStep;
	
  // Inner samples on the tab.
  vec3 rgbSampleNeg = texture(renderedTex, texCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
  vec3 rgbSamplePos = texture(renderedTex, texCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;
  vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

  // Outer samples on the tab.
  vec3 rgbSampleNegOuter = texture(renderedTex, texCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
  vec3 rgbSamplePosOuter = texture(renderedTex, texCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
  vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   

  float lumaFourTab = dot(rgbFourTab, TO_LUMA);
	
  // Are outer samples of the tab beyond the edge?
  if (lumaFourTab < lumaMin || lumaFourTab > lumaMax) {
    outColor = rgbTwoTab;
  } else {
    outColor = rgbFourTab;
  }

  // Show edges for debug purposes.
  // outColor.r = 1.0;
}
