//------------------------------------------------------------------------------
//  gather.fx
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/techniques.fxh"

vec4 FogDistances = vec4(0.0, 2500.0, 0.0, 1.0);
vec4 FogColor = vec4(0.5, 0.5, 0.63, 0.0);

/// Declaring used textures
sampler2D LightTexture;
sampler2D DepthTexture;
sampler2D EmissiveTexture;
sampler2D SSSTexture;
sampler2D SSAOTexture;

samplerstate GatherSampler
{
	Samplers = { LightTexture, SSSTexture, SSAOTexture, DepthTexture };
	Filter = Point;
	AddressU = Border;
	AddressV = Border;
	BorderColor = { 0, 0, 0, 0 };
};

state GatherState
{
	CullMode = Back;
	DepthEnabled = false;
	DepthWrite = false;
};

//------------------------------------------------------------------------------
/**
    Compute fogging given a sampled fog intensity value from the depth
    pass and a fog color.
*/
vec4 
psFog(float fogDepth, vec4 color)
{
    float fogIntensity = clamp((FogDistances.y - fogDepth) / (FogDistances.y - FogDistances.x), FogColor.a, 1.0);
    return vec4(lerp(FogColor.rgb, color.rgb, fogIntensity), color.a);
}

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
[earlydepth]
shader
void
psMain(in vec2 UV,
	[color0] out vec4 MergedColor) 
{
	vec4 sssLight = DecodeHDR(texture(SSSTexture, UV));
	vec4 light = DecodeHDR(texture(LightTexture, UV));
	vec4 emissiveColor = texture(EmissiveTexture, UV);
	float ssao = texture(SSAOTexture, UV).r;
	
	// blend non-blurred light with SSS light
	light.rgb = lerp(light.rgb + emissiveColor.rgb, sssLight.rgb, sssLight.a) * (1.0f - ssao);	
	vec4 color = light;
	
	float depth = texture(DepthTexture, UV).r;
	color = psFog(depth, color);
	MergedColor = EncodeHDR(color);
}

//------------------------------------------------------------------------------
/**
*/
PostEffect(vsMain(), psMain(), GatherState);
