//------------------------------------------------------------------------------
//  frameposteffect.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/frameposteffect.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/shaderserver.h"

namespace Frame
{
__ImplementClass(Frame::FramePostEffect, 'FPEF', Frame::FramePassBase);

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FramePostEffect::FramePostEffect()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FramePostEffect::~FramePostEffect()
{     
    // empty
}

//------------------------------------------------------------------------------
/**
    This setsup the quad mesh for rendering the fullscreen quad.
*/
void
FramePostEffect::Setup()
{
    n_assert(this->renderTarget.isvalid());
                            
    // setup the fullscreen quad renderer      
    SizeT w = this->renderTarget->GetWidth();
    SizeT h = this->renderTarget->GetHeight();
    this->drawFullScreenQuad.Setup(w, h);
}

//------------------------------------------------------------------------------
/**
*/
void
FramePostEffect::Discard()
{
    FramePassBase::Discard();
    this->drawFullScreenQuad.Discard();
#if NEBULA3_ENABLE_PROFILING
    _discard_timer(debugTimer);
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
FramePostEffect::Render(IndexT frameIndex)
{
#if NEBULA3_ENABLE_PROFILING
    _start_timer(this->debugTimer);
#endif

	n_assert(this->renderTarget.isvalid() || this->multipleRenderTarget.isvalid() || this->renderTargetCube.isvalid());
    RenderDevice* renderDevice = RenderDevice::Instance();
	ShaderServer* shaderServer = ShaderServer::Instance();

    // update render target
    if (this->renderTarget.isvalid())
    {
        // update render targets
		this->renderTarget->SetClearFlags(this->clearFlags);
		this->renderTarget->SetClearDepth(this->clearDepth);
		this->renderTarget->SetClearStencil(this->clearStencil);
		this->renderTarget->SetClearColor(this->clearColor);
    }
	else if (this->renderTargetCube.isvalid())
	{
		n_assert(!this->renderTarget.isvalid());
		n_assert(!this->multipleRenderTarget.isvalid());

		// update render targets
		this->renderTargetCube->SetClearFlags(this->clearFlags);
		this->renderTargetCube->SetClearDepth(this->clearDepth);
		this->renderTargetCube->SetClearStencil(this->clearStencil);
		this->renderTargetCube->SetClearColor(this->clearColor);
	}
	else if (this->multipleRenderTarget.isvalid())
	{
		// ignore clear flags
		n_assert(!this->renderTarget.isvalid());
		n_assert(!this->renderTargetCube.isvalid());
	}

    // activate shader
    shaderServer->SetActiveShader(this->shader);
    this->shader->Apply();

    // apply shader variables
    this->shader->BeginUpdate();
    IndexT varIndex;
    for (varIndex = 0; varIndex < this->shaderVariables.Size(); varIndex++)
    {
        this->shaderVariables[varIndex]->Apply();
    }

	// bind render target, either 2D, MRT, or Cube
    if (this->renderTarget.isvalid())
    {
		n_assert(!this->multipleRenderTarget.isvalid());
		n_assert(!this->renderTargetCube.isvalid());
        renderDevice->BeginPass(this->renderTarget, this->shader);
    }
	else if (this->multipleRenderTarget.isvalid())
    {
		n_assert(!this->renderTarget.isvalid());
		n_assert(!this->renderTargetCube.isvalid());
        renderDevice->BeginPass(this->multipleRenderTarget, this->shader);
    }     
	else if (this->renderTargetCube.isvalid())
	{
		n_assert(!this->renderTarget.isvalid());
		n_assert(!this->multipleRenderTarget.isvalid());		
		renderDevice->BeginPass(this->renderTargetCube, this->shader);
	}
	else
	{
		n_error("FramePostEffect::Render() : No render targets assigned!");
	}
    this->shader->EndUpdate();

	// draw
    this->shader->Commit();
    this->drawFullScreenQuad.Draw();

    IndexT batchIndex;	
    for (batchIndex = 0; batchIndex < this->batches.Size(); batchIndex++)
    {
        this->batches[batchIndex]->Render(frameIndex);
    }
    renderDevice->EndPass();

#if NEBULA3_ENABLE_PROFILING
    _stop_timer(this->debugTimer);
#endif
}

//------------------------------------------------------------------------------
/**
*/
void 
FramePostEffect::OnWindowResize(SizeT width, SizeT height)
{
    n_assert(this->renderTarget.isvalid());
    n_assert(this->drawFullScreenQuad.IsValid());

    this->drawFullScreenQuad.Discard();
    SizeT w = this->renderTarget->GetWidth();
    SizeT h = this->renderTarget->GetHeight();
    this->drawFullScreenQuad.Setup(w, h);
}

//------------------------------------------------------------------------------
/**
*/
#if NEBULA3_ENABLE_PROFILING
void 
FramePostEffect::SetFramePostEffectDebugTimer( const Util::String& name )
{
    this->debugTimer = Debug::DebugTimer::Create();
	this->debugTimer->Setup(name, "Frame shaders");
}


#endif
} // namespace Frame
