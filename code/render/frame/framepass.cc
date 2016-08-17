//------------------------------------------------------------------------------
//  framepass.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framepass.h"
#include "coregraphics/renderdevice.h"

namespace Frame
{
__ImplementClass(Frame::FramePass, 'FPSS', Frame::FramePassBase);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FramePass::FramePass()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
FramePass::~FramePass()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FramePass::Discard()
{
    FramePassBase::Discard();
#if NEBULA3_ENABLE_PROFILING
    _discard_timer(debugTimer);
#endif
}

//------------------------------------------------------------------------------
/**
*/
void
FramePass::Render(IndexT frameIndex)
{
#if NEBULA3_ENABLE_PROFILING
    _start_timer(this->debugTimer);
#endif

    n_assert(this->renderTarget.isvalid() || this->multipleRenderTarget.isvalid() || this->renderTargetCube.isvalid());
    RenderDevice* renderDevice = RenderDevice::Instance();

    if (this->renderTarget.isvalid())
    {
        n_assert(!this->multipleRenderTarget.isvalid());
        n_assert(!this->renderTargetCube.isvalid());

        // update render targets
        this->renderTarget->SetClearFlags(this->clearFlags);
		this->renderTarget->SetClearDepth(this->clearDepth);
		this->renderTarget->SetClearStencil(this->clearStencil);
        this->renderTarget->SetClearColor(this->clearColor);
		renderDevice->BeginPass(this->renderTarget);
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
		renderDevice->BeginPass(this->renderTargetCube);
    }
    else if (this->multipleRenderTarget.isvalid())
    {
		// ignore clear flags
        n_assert(!this->renderTarget.isvalid());
        n_assert(!this->renderTargetCube.isvalid());
		renderDevice->BeginPass(this->multipleRenderTarget);
    }
	else
	{
		n_error("FramePass::Render() : No render targets assigned!");
	}

	// if we have a shader, apply its substate
	if (this->shader.isvalid()) this->shader->Commit();

	// render batches...
    IndexT batchIndex;
    for (batchIndex = 0; batchIndex < this->batches.Size(); batchIndex++)
    {
        FRAME_LOG("    FramePass::Render() %s  batch: %d", this->GetName().AsString().AsCharPtr(), batchIndex);
        this->batches[batchIndex]->Render(frameIndex);
        FRAME_LOG(" ");
    }
    renderDevice->EndPass();

#if NEBULA3_ENABLE_PROFILING
    _stop_timer(this->debugTimer);
#endif
}

//------------------------------------------------------------------------------
/**
*/
#if NEBULA3_ENABLE_PROFILING
void 
FramePass::SetFramePassDebugTimer(const Util::String& name)
{
    this->debugTimer = Debug::DebugTimer::Create();
    this->debugTimer->Setup(name, "Frame shaders");
}
#endif
} // namespace Frame
