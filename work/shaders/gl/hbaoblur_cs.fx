//------------------------------------------------------------------------------
//  hbaoblur.fx
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"

float PowerExponent = 1.0f;
float BlurFalloff;
float BlurDepthThreshold;

sampler2D HBAOBuffer;
write rg16f image2D HBAOBlurred;
write r16f image2D HBAOFinal;

samplerstate HBAOSampler
{
	Samplers = { HBAOBuffer };
	AddressU = Border;
	AddressV = Border;
	Filter = Point;
	BorderColor = { 0,0,0,0 };
};

state HBAOBlurState
{
	CullMode = Back;
	DepthEnabled = false;
	DepthWrite = false;
};

#define KERNEL_RADIUS 16
#define HALF_KERNEL_RADIUS (KERNEL_RADIUS/2)

#define ROW_TILE_WIDTH 320
#define SHARED_MEM_SIZE KERNEL_RADIUS + ROW_TILE_WIDTH + KERNEL_RADIUS
groupshared vec2 SharedMemory[SHARED_MEM_SIZE];

//----------------------------------------------------------------------------------
float CrossBilateralWeight(float r, float d, float d0)
{
    return exp2(-r*r*BlurFalloff) * float(abs(d - d0) < BlurDepthThreshold);
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = SHARED_MEM_SIZE
[localsizey] = 1
shader
void
csMainX() 
{
	// get full resolution and inverse full resolution
	vec2 size = textureSize(HBAOBuffer, 0);
	vec2 inverseSize = 1 / size;
	
	// calculate offsets
	const float               row = float(gl_WorkGroupID.y);
	const float         tileStart = float(gl_WorkGroupID.x) * ROW_TILE_WIDTH;
	const float           tileEnd = tileStart + ROW_TILE_WIDTH;
	const float        apronStart = tileStart - KERNEL_RADIUS;
	const float          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const float x = apronStart + float(gl_LocalInvocationID.x) + 0.5f;
	const float y = row;
	const vec2 uv = (vec2(x,y) + 0.5f) * inverseSize;
	SharedMemory[gl_LocalInvocationID.x] = textureLod(HBAOBuffer, uv, 0).rg;
	groupMemoryBarrier();
	
	const float writePos = tileStart + float(gl_LocalInvocationID.x);
	const float tileEndClamped = min(tileEnd, size.x);
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		vec2 uv = vec2(writePos, y);
		vec2 AoDepth = texelFetch(HBAOBuffer, ivec2(uv), 0).rg;
		float ao_total = AoDepth.x;
		float center_d = AoDepth.y;
		float w_total = 1;
		float i;

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + (-KERNEL_RADIUS + 0.5f);
		    uint j = 2*uint(i) + gl_LocalInvocationID.x;
		    vec2 samp = SharedMemory[j];
		    float w = CrossBilateralWeight(r, samp.y, center_d);
		    ao_total += w * samp.x;
		    w_total += w;
		}

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + 1.5f;
		    uint j = 2*uint(i) + gl_LocalInvocationID.x + KERNEL_RADIUS + 1;
		    vec2 samp = SharedMemory[j];
		    float w = CrossBilateralWeight(r, samp.y, center_d);
		    ao_total += w * samp.x;
		    w_total += w;
		}
		
		float ao = ao_total / w_total;
		imageStore(HBAOBlurred, int2(writePos, gl_WorkGroupID.y), vec4(ao, center_d, 0, 0));
	}
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = SHARED_MEM_SIZE
[localsizey] = 1
shader
void
csMainY() 
{
	// get full resolution and inverse full resolution
	vec2 size = textureSize(HBAOBuffer,  0);
	vec2 inverseSize = 1 / size;
	
	// calculate offsets
	const float               col = float(gl_WorkGroupID.y);
	const float         tileStart = float(gl_WorkGroupID.x) * ROW_TILE_WIDTH;
	const float           tileEnd = tileStart + ROW_TILE_WIDTH;
	const float        apronStart = tileStart - KERNEL_RADIUS;
	const float          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const float x = col;
	const float y = apronStart + float(gl_LocalInvocationID.x) + 0.5f;
	const vec2 uv = (vec2(x,y) + 0.5f) * inverseSize;
	SharedMemory[gl_LocalInvocationID.x] = textureLod(HBAOBuffer, uv, 0).rg;
	groupMemoryBarrier();
	
	const float writePos = tileStart + float(gl_LocalInvocationID.x);
	const float tileEndClamped = min(tileEnd, size.x);
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		vec2 uv = vec2(x, writePos);
		vec2 AoDepth = texelFetch(HBAOBuffer, ivec2(uv), 0).rg;
		float ao_total = AoDepth.x;
		float center_d = AoDepth.y;
		float w_total = 1;
		float i;

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + (-KERNEL_RADIUS + 0.5f);
		    uint j = 2*uint(i) + gl_LocalInvocationID.x;
		    vec2 samp = SharedMemory[j];
		    float w = CrossBilateralWeight(r, samp.y, center_d);
		    ao_total += w * samp.x;
		    w_total += w;
		}

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + 1.5f;
		    uint j = 2*uint(i) + gl_LocalInvocationID.x + KERNEL_RADIUS + 1;
		    vec2 samp = SharedMemory[j];
		    float w = CrossBilateralWeight(r, samp.y, center_d);
		    ao_total += w * samp.x;
		    w_total += w;
		}
		
		float ao = ao_total / w_total;
		imageStore(HBAOFinal, int2(gl_WorkGroupID.y, writePos), vec4(pow(ao, PowerExponent),0,0,0));
	}
}

//------------------------------------------------------------------------------
/**
*/
program BlurX [ string Mask = "X"; ]
{
	ComputeShader = csMainX();
};

program BlurY [ string Mask = "Y"; ]
{
	ComputeShader = csMainY();
};