//------------------------------------------------------------------------------
//  blur_2d_rgba16_cs.fx
//
//  (C) 2016 Gustav Sterbrant
//------------------------------------------------------------------------------

#define IMAGE_IS_RGBA16 1
#define KERNEL_RADIUS 5
#include "lib/blur_2d_cs.fxh"

//------------------------------------------------------------------------------
/**
*/
program BlurX [ string Mask = "Alt0"; ]
{
	ComputeShader = csMainX();
};

program BlurY [ string Mask = "Alt1"; ]
{
	ComputeShader = csMainY();
};