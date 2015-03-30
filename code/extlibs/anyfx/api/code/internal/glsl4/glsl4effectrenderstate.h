#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::GLSL4EffectRenderState
    
    GLSL4 backend for EffectRenderState, implements InternalEffectRenderState.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "internal/internaleffectrenderstate.h"
#include "GL/glew.h"
namespace AnyFX
{
class GLSL4EffectRenderState : public InternalEffectRenderState
{
public:
	/// constructor
	GLSL4EffectRenderState();
	/// destructor
	virtual ~GLSL4EffectRenderState();

private:
	
	/// applies opengl4 render state
	void Apply();
}; 

struct GLSL4GlobalRenderState
{
    // rasterizer stuff
    static unsigned      polygonMode;
    static unsigned      cullMode;
    static unsigned      cullFace;
    static bool          depthEnabled;
    static bool          depthWriteEnabled;
	static bool			 depthClampEnabled;
	static bool			 separateBlendEnabled;
    static unsigned      depthFunc;
    static bool          scissorEnabled;
    static bool          multisampleEnabled;
    static bool          alphaToCoverageEnabled;
	static bool			 polygonOffsetEnabled;
	static int			 polygonOffsetFactor;
	static int			 polygonOffsetUnits;

    // stencil stuff
    static bool          stencilEnabled;
    static unsigned      stencilBackOp[3];
    static unsigned      stencilFrontOp[3];
    static unsigned      stencilFunc[2];     // 0 is back, 1 is front
    static unsigned      stencilRef[2];      // 0 is back ref, 1 is front ref
    static unsigned      stencilWriteMask;
    static unsigned      stencilReadMask;

    // blend state stuff
    static bool          blendEnabled[InternalEffectRenderState::MaxNumRenderTargets];
    static unsigned      srcBlends[InternalEffectRenderState::MaxNumRenderTargets];
    static unsigned      dstBlends[InternalEffectRenderState::MaxNumRenderTargets];
    static unsigned      alphaSrcBlends[InternalEffectRenderState::MaxNumRenderTargets];
    static unsigned      alphaDstBlends[InternalEffectRenderState::MaxNumRenderTargets];
    static unsigned      blendOps[InternalEffectRenderState::MaxNumRenderTargets];
    static unsigned      alphaBlendOps[InternalEffectRenderState::MaxNumRenderTargets];
};

} // namespace AnyFX
//------------------------------------------------------------------------------