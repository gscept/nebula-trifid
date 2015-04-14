//------------------------------------------------------------------------------
//  hbaoblur_cs.fx
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"

float PowerExponent = 1.0f;
float BlurFalloff;
float BlurDepthThreshold;

sampler2D HBAOReadLinear;
sampler2D HBAOReadPoint;

samplerstate LinearState
{
	Samplers = {HBAOReadLinear};
	Filter = Linear;
	AddressU = Clamp;
	AddressV = Clamp;
};

samplerstate PointState
{
	Samplers = {HBAOReadPoint};
	Filter = Point;
	AddressU = Clamp;
	AddressV = Clamp;
};


write rg16f image2D HBAORG;
write r16f image2D HBAOR;

#define KERNEL_RADIUS 16
#define HALF_KERNEL_RADIUS (KERNEL_RADIUS/2.0f)

#define HBAO_TILE_WIDTH 320
#define SHARED_MEM_SIZE (KERNEL_RADIUS + HBAO_TILE_WIDTH + KERNEL_RADIUS)
groupshared vec2 SharedMemory[SHARED_MEM_SIZE];

//------------------------------------------------------------------------------
/**
*/
float
CrossBilateralWeight(float r, float d, float d0)
{
    return exp2(-r*r*BlurFalloff) * float(abs(d - d0) < BlurDepthThreshold);
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = SHARED_MEM_SIZE
shader
void
csMainX() 
{
	// get full resolution and inverse full resolution
	vec2 size = textureSize(HBAOReadLinear, 0);
	vec2 inverseSize = 1 / size;
	
	// calculate offsets
	const int         tileStart = int(gl_WorkGroupID.x) * HBAO_TILE_WIDTH;
	const int           tileEnd = tileStart + HBAO_TILE_WIDTH;
	const int        apronStart = tileStart - KERNEL_RADIUS;
	const int          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const int x = apronStart + int(gl_LocalInvocationID.x);
	const int y = int(gl_WorkGroupID.y);
	const vec2 uv = vec2(x, y) * inverseSize;
	SharedMemory[gl_LocalInvocationID.x] = textureLod(HBAOReadLinear, uv, 0).xy;
	groupMemoryBarrier();
	
	const uint writePos = tileStart + gl_LocalInvocationID.x;
	const uint tileEndClamped = min(tileEnd, int(size.x));
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		vec2 uv = vec2(writePos, y) * inverseSize;
		vec2 AoDepth = textureLod(HBAOReadPoint, uv, 0).xy;
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
		imageStore(HBAORG, int2(writePos, gl_WorkGroupID.y), vec4(ao, center_d, 0, 0));
	}
}

//------------------------------------------------------------------------------
/**
*/
[localsizex] = SHARED_MEM_SIZE
shader
void
csMainY() 
{
	// get full resolution and inverse full resolution
	vec2 size = textureSize(HBAOReadLinear, 0);
	vec2 inverseSize = 1 / size;
	
	// calculate offsets
	const int         tileStart = int(gl_WorkGroupID.x) * HBAO_TILE_WIDTH;
	const int           tileEnd = tileStart + HBAO_TILE_WIDTH;
	const int        apronStart = tileStart - KERNEL_RADIUS;
	const int          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const int x = int(gl_WorkGroupID.y);
	const int y = apronStart + int(gl_LocalInvocationID.x);
	const vec2 uv = vec2(x, y) * inverseSize;
	SharedMemory[gl_LocalInvocationID.x] = textureLod(HBAOReadLinear, uv, 0).xy;
	groupMemoryBarrier();
	
	const uint writePos = tileStart + gl_LocalInvocationID.x;
	const uint tileEndClamped = min(tileEnd, int(size.x));
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		vec2 uv = vec2(x, writePos) * inverseSize;
		vec2 AoDepth = textureLod(HBAOReadPoint, uv, 0).xy;
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
		imageStore(HBAOR, int2(gl_WorkGroupID.y, writePos), vec4(pow(ao, PowerExponent),0,0,0));
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