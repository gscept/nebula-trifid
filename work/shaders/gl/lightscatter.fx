//------------------------------------------------------------------------------
//  lightscatter.fx
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"
	
float Density = 0.93f;
float Decay = 0.91f;
float Weight = 0.20f;
float Exposure = 0.21f;
vec2 LightPos = vec2(0.5f, 0.5f);

#define NUM_GLOBAL_SAMPLES 16
#define NUM_LOCAL_SAMPLES 16

/// Declaring used textures
sampler2D ColorSource;

samplerstate ColorSampler
{
	Samplers = { ColorSource };
	Filter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};

state LightScatterState
{
	CullMode = Back;
	DepthWrite = false;
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(in vec3 position,
	in vec2 uv,
	out vec2 UV) 
{
	gl_Position = vec4(position, 1);
	UV = FlipY(uv);
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psMainLocal(in vec2 UV,
	[color0] out vec4 Color) 
{	
	vec2 screenUV = UV;
	vec2 lightScreenPos = LightPos;
	lightScreenPos.y = 1 - lightScreenPos.y;
	vec2 deltaTexCoord = vec2(screenUV - lightScreenPos);
	deltaTexCoord *= 1.0f / NUM_LOCAL_SAMPLES * Density;
	vec4 color = DecodeHDR(textureLod(ColorSource, screenUV, 0));
	float illuminationDecay = 1.0f;
	
	for (int i = 0; i < NUM_LOCAL_SAMPLES; i++)
	{
		screenUV -= deltaTexCoord;
		vec4 sampleColor = DecodeHDR(textureLod(ColorSource, screenUV, 0));
		sampleColor *= illuminationDecay * Weight;
		color += sampleColor;
		illuminationDecay *= Decay;
	}
	color *= Exposure;
	
	Color = EncodeHDR(vec4(color.rgb, 1));
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psMainGlobal(in vec2 UV,
	[color0] out vec4 Color) 
{
	vec2 screenUV = UV; 
	vec2 lightScreenPos = LightPos;
	lightScreenPos.y = 1 - lightScreenPos.y;
	vec2 deltaTexCoord = vec2(screenUV - lightScreenPos);
	deltaTexCoord *= 1.0f / NUM_GLOBAL_SAMPLES * Density;
	vec4 color = DecodeHDR(textureLod(ColorSource, screenUV, 0));
	float illuminationDecay = 1.0f;
	
	for (int i = 0; i < NUM_GLOBAL_SAMPLES; i++)
	{
		screenUV -= deltaTexCoord;
		vec4 sampleColor = DecodeHDR(textureLod(ColorSource, screenUV, 0));
		sampleColor *= illuminationDecay * Weight;
		color += sampleColor;
		illuminationDecay *= Decay;
	}
	color *= Exposure;
	
	Color = EncodeHDR(vec4(color.rgb, 1));
}

//------------------------------------------------------------------------------
/**
*/
//SimpleTechnique(LocalScatter, "Light|Local", vsMain(), psMainLocal(), LightScatterState);
SimpleTechnique(GlobalScatter, "Global", vsMain(), psMainLocal(), LightScatterState);
