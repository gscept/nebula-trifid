//------------------------------------------------------------------------------
/**
  blur_2d_cs.fxh

	Blurring kernel used for 2D textures. Implements a double pass X-Y blur with a defined kernel size.
	First pass samples from a render-able texture (Alt0) and works in the X-axis.
	The second pass resamples from the same texture and blurs in the Y-axis.


	Include this header and then define if you want an RGBA16F, RG16F or RG32F image as input.

  (C) 2016 Gustav Sterbrant
*/
#include "lib/shared.fxh"

#if IMAGE_IS_RGBA16F
#define IMAGE_FORMAT_TYPE rgba16f
#define IMAGE_LOAD_VEC vec4
#define IMAGE_LOAD_SWIZZLE(vec) vec.xyzw
#define RESULT_TO_VEC4(vec) vec
#elif IMAGE_IS_RG16F
#define IMAGE_FORMAT_TYPE rg16f
#define IMAGE_LOAD_VEC vec2
#define IMAGE_LOAD_SWIZZLE(vec) vec.xy
#define RESULT_TO_VEC4(vec) vec4(vec.xy, 0, 0)
#elif IMAGE_IS_RG32F
#define IMAGE_FORMAT_TYPE rg32f
#define IMAGE_LOAD_VEC vec2
#define IMAGE_LOAD_SWIZZLE(vec) vec.xy
#define RESULT_TO_VEC4(vec) vec4(vec.xy, 0, 0)
#endif

samplerstate InputSampler
{
	Filter = Point;
};

textureHandle InputImage;
readwrite IMAGE_FORMAT_TYPE image2D BlurImage;
#define INV_LN2 1.44269504f

#define KERNEL_RADIUS 16
#define KERNEL_RADIUS_FLOAT 16.0f
#define HALF_KERNEL_RADIUS (KERNEL_RADIUS/2.0f)

#define BLUR_TILE_WIDTH 320
#define SHARED_MEM_SIZE (KERNEL_RADIUS + BLUR_TILE_WIDTH + KERNEL_RADIUS)
groupshared IMAGE_LOAD_VEC SharedMemory[SHARED_MEM_SIZE];

//------------------------------------------------------------------------------
/**
	Calculate bilateral weight function, which is
	
	Fcolor(abs(I[xi] - I[x])) * Fcoord(abs(xi - x))
	The coords are 1D for our kernel, or rather the x * -x.
*/
IMAGE_LOAD_VEC
BilateralWeight(IMAGE_LOAD_VEC p, IMAGE_LOAD_VEC pi, float u)
{
	const float sigma = (33 + 1) * 0.5f;
	const float falloff = INV_LN2 / (2.0f * sigma * sigma);
	IMAGE_LOAD_VEC pixelDiff = abs(pi - p);
    return exp2(-u*u) * pixelDiff;
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
	ivec2 size = imageSize(BlurImage);
	
	// calculate offsets
	const uint         tileStart = int(gl_WorkGroupID.x) * BLUR_TILE_WIDTH;
	const uint           tileEnd = tileStart + BLUR_TILE_WIDTH;
	const uint        apronStart = tileStart - KERNEL_RADIUS;
	const uint          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const uint x = apronStart + gl_LocalInvocationID.x;
	const uint y = gl_WorkGroupID.y;
	
	// load into workgroup saved memory, this allows us to use the original pixel even though 
	// we might have replaced it with the result from this thread!
	SharedMemory[gl_LocalInvocationID.x] = IMAGE_LOAD_SWIZZLE(fetch2D(InputImage, InputSampler, ivec2(x, y), 0));
	//barrier();
	
	const uint writePos = tileStart + gl_LocalInvocationID.x;
	const uint tileEndClamped = min(tileEnd, uint(size.x));
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		IMAGE_LOAD_VEC color = IMAGE_LOAD_SWIZZLE(fetch2D(InputImage, InputSampler, ivec2(writePos, y), 0));
		IMAGE_LOAD_VEC blurTotal = color;
		IMAGE_LOAD_VEC wTotal = IMAGE_LOAD_VEC(1);
		float i;

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + (-KERNEL_RADIUS_FLOAT + 0.5f);
		    uint j = 2*uint(i) + gl_LocalInvocationID.x;
		    IMAGE_LOAD_VEC samp = SharedMemory[j];
		    IMAGE_LOAD_VEC w = BilateralWeight(color, samp, r);
			blurTotal += w * samp;
		    wTotal += w;
		}

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + 1.5f;
		    uint j = 2*uint(i) + gl_LocalInvocationID.x + KERNEL_RADIUS + 1;
		    IMAGE_LOAD_VEC samp = SharedMemory[j];
		    IMAGE_LOAD_VEC w = BilateralWeight(color, samp, r);
			blurTotal += w * samp;
		    wTotal += w;
		}
		
		IMAGE_LOAD_VEC blur = blurTotal / wTotal;
		imageStore(BlurImage, ivec2(writePos, gl_WorkGroupID.y), RESULT_TO_VEC4(blur));
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
	ivec2 size = imageSize(BlurImage);
	
	// calculate offsets
	const uint         tileStart = int(gl_WorkGroupID.x) * BLUR_TILE_WIDTH;
	const uint           tileEnd = tileStart + BLUR_TILE_WIDTH;
	const uint        apronStart = tileStart - KERNEL_RADIUS;
	const uint          apronEnd = tileEnd   + KERNEL_RADIUS;
	
	const uint x = gl_WorkGroupID.y;
	const uint y = apronStart + gl_LocalInvocationID.x;
	
	// load into workgroup saved memory, this allows us to use the original pixel even though 
	// we might have replaced it with the result from this thread!
	SharedMemory[gl_LocalInvocationID.x] = IMAGE_LOAD_SWIZZLE(imageLoad(BlurImage, ivec2(x, y)));
	barrier();
	
	const uint writePos = tileStart + gl_LocalInvocationID.x;
	const uint tileEndClamped = min(tileEnd, uint(size.y));
	
	if (writePos < tileEndClamped)
	{
		// Fetch (ao,z) at the kernel center
		IMAGE_LOAD_VEC color = IMAGE_LOAD_SWIZZLE(imageLoad(BlurImage, ivec2(x, writePos)));
		IMAGE_LOAD_VEC blurTotal = color;
		IMAGE_LOAD_VEC wTotal = IMAGE_LOAD_VEC(1);
		float i;

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + (-KERNEL_RADIUS_FLOAT + 0.5f);
		    uint j = 2*uint(i) + gl_LocalInvocationID.x;
		    IMAGE_LOAD_VEC samp = SharedMemory[j];
		    IMAGE_LOAD_VEC w = BilateralWeight(color, samp, r);
			blurTotal += w * samp;
		    wTotal += w;
		}

		#pragma unroll
		for (i = 0; i < HALF_KERNEL_RADIUS; ++i)
		{
		    // Sample the pre-filtered data with step size = 2 pixels
		    float r = 2.0f*i + 1.5f;
		    uint j = 2*uint(i) + gl_LocalInvocationID.x + KERNEL_RADIUS + 1;
		    IMAGE_LOAD_VEC samp = SharedMemory[j];
		    IMAGE_LOAD_VEC w = BilateralWeight(color, samp, r);
			blurTotal += w * samp;
		    wTotal += w;
		}
		
		IMAGE_LOAD_VEC blur = blurTotal / wTotal;
		imageStore(BlurImage, ivec2(gl_WorkGroupID.y, writePos), RESULT_TO_VEC4(blur));
	}
}