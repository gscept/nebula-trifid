//------------------------------------------------------------------------------
//  brightpass_cs.fx
//  (C) 2016 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/shared.fxh"
#include "lib/techniques.fxh"

#define IMAGE_IS_RGBA16F 1
#define READ_WRITE_SEPARATE 1
#include "lib/blur_2d_cs.fxh"

//------------------------------------------------------------------------------
/**
*/
[localsizex] = SHARED_MEM_SIZE
shader
void
csMain() 
{
	// calculate offsets
	uint x, y, start, end;
	ComputePixelX(BLUR_TILE_WIDTH, HALF_KERNEL_RADIUS, gl_WorkGroupID.xy, gl_LocalInvocationID.xy, x, y, start, end);
	
	const uint writePos = start + gl_LocalInvocationID.x;
	GaussianXKernel(ivec2(writePos, y), ivec2(x,y) * 2, gl_WorkGroupID, gl_LocalInvocationID, uvec2(start, end));
}

//------------------------------------------------------------------------------
/**
*/
program PostEffect [ string Mask = "Alt0"; ]
{
	ComputeShader = csMain();
};
