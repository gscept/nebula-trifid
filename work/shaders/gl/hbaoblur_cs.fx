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

readwrite rg16f image2D HBAO0;
readwrite rg16f image2D HBAO1;
readwrite r16f image2D HBAO2;

#define KERNEL_RADIUS 16
#define HALF_KERNEL_RADIUS (KERNEL_RADIUS/2)

#define HBAO_TILE_WIDTH 320
#define SHARED_MEM_SIZE KERNEL_RADIUS + HBAO_TILE_WIDTH + KERNEL_RADIUS
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
[localsizey] = 1
shader
void
csMainX() 
{
	// get full resolution and inverse full resolution
	vec2 size = imageSize(HBAO0);
	vec2 inverseSize = 1 / size;
	
	// calculate offsets
	const uint         tileStart = gl_WorkGroupID.x * HBAO_TILE_WIDTH;
	const uint           tileEnd = tileStart + HBAO_TILE_WIDTH;
	const uint        apronStart = tileStart - KERNEL_RADIUS;
	const uint          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const uint x = apronStart + gl_LocalInvocationID.x;
	const uint y = gl_WorkGroupID.y;
	const ivec2 uv = ivec2(x, y);
	SharedMemory[gl_LocalInvocationID.x] = imageLoad(HBAO0, uv).rg;
	memoryBarrierShared();
	
	const uint writePos = tileStart + gl_LocalInvocationID.x;
	const uint tileEndClamped = min(tileEnd, int(size.x));
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		ivec2 uv = ivec2(writePos, y);
		vec2 AoDepth = imageLoad(HBAO0, uv).rg;
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
		imageStore(HBAO1, int2(writePos, gl_WorkGroupID.y), vec4(ao, center_d, 0, 0));
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
	vec2 size = imageSize(HBAO0);
	vec2 inverseSize = 1 / size;
	
	// calculate offsets
	const uint         tileStart = gl_WorkGroupID.x * HBAO_TILE_WIDTH;
	const uint           tileEnd = tileStart + HBAO_TILE_WIDTH;
	const uint        apronStart = tileStart - KERNEL_RADIUS;
	const uint          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const uint x = gl_WorkGroupID.y;
	const uint y = apronStart + gl_LocalInvocationID.x;
	const ivec2 uv = ivec2(x, y);
	SharedMemory[gl_LocalInvocationID.x] = imageLoad(HBAO0, uv).rg;
	memoryBarrierShared();
	
	const uint writePos = tileStart + gl_LocalInvocationID.x;
	const uint tileEndClamped = min(tileEnd, int(size.y));
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		ivec2 uv = ivec2(x, writePos);
		vec2 AoDepth = imageLoad(HBAO0, uv).rg;
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
		imageStore(HBAO2, int2(gl_WorkGroupID.y, writePos), vec4(pow(ao, PowerExponent),0,0,0));
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