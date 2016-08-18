//------------------------------------------------------------------------------
//  red.fx
//  (C) 2016 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"
#include "lib/shared.fxh"

sampler2D CopyBuffer;
textureHandle TestHandle;
float multiplier = 0.5f;
float multiplier2 = 1;
float multiplier3 = 1;

shared varblock ColorBlock
{
vec4 bs1;
vec4 bs2;
vec4 bs3;
mat4 ColorMatrix;
mat4 ColorMatrix1;
mat4 ColorMatrix2;
};

samplerstate CopySampler 
{
	Samplers = { CopyBuffer };
	Filter = Point;
};

state CopyState
{
	CullMode = None;
	DepthEnabled = false;
	DepthWrite = false;
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(
	[slot=0] in vec3 position,
	[slot=2] in vec2 uv,
	out vec2 UV) 
{
    gl_Position = vec4(position, 1);
    UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psMain(in vec2 uv, 
	[color0] out vec4 Color)
{
	Color = textureLod(CopyBuffer, uv, 0); 
}

//------------------------------------------------------------------------------
/**
*/
PostEffect(vsMain(), psMain(), CopyState);
