//------------------------------------------------------------------------------
//  frameshapes.cc
//  (C) 2011 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/frameshapes.h"
#include "coregraphics/shaperenderer.h"
#include "coregraphics/renderdevice.h"

namespace Frame
{
__ImplementClass(Frame::FrameShapes, 'FRMS', Core::RefCounted);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FrameShapes::FrameShapes()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameShapes::~FrameShapes()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShapes::Discard()
{
	FramePassBase::Discard();
	_discard_timer(debugTimer);
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameShapes::Render()
{
	_start_timer(this->debugTimer);

	n_assert(this->renderTarget.isvalid() ||this->multipleRenderTarget.isvalid());
	RenderDevice* renderDevice = RenderDevice::Instance();

	if (this->renderTarget.isvalid())
	{
		n_assert(!this->multipleRenderTarget.isvalid());
		// update render targets
		this->renderTarget->SetClearFlags(this->clearFlags);
		this->renderTarget->SetClearColor(this->clearColor);
		this->renderTarget->SetClearDepth(this->clearDepth);
		this->renderTarget->SetClearStencil(this->clearStencil);  
	}  
	else if (this->multipleRenderTarget.isvalid())
	{
		n_assert(!this->renderTarget.isvalid());
	}

	// apply shader variables
	IndexT varIndex;
	for (varIndex = 0; varIndex < this->shaderVariables.Size(); varIndex++)
	{
		this->shaderVariables[varIndex]->Apply();
	}

	// render batches...
	if (this->renderTarget.isvalid())
	{
		renderDevice->BeginPass(this->renderTarget, NULL);
	}
	else
	{
		renderDevice->BeginPass(this->multipleRenderTarget, NULL);	
	}

	ShapeRenderer::Instance()->DrawShapes();

	renderDevice->EndPass();

	_stop_timer(this->debugTimer);
}

//------------------------------------------------------------------------------
/**
*/
#if NEBULA3_ENABLE_PROFILING
void 
FrameShapes::SetFrameShapesPassDebugTimer(const Util::String& name)
{
	this->debugTimer = Debug::DebugTimer::Create();
	this->debugTimer->Setup(name);
}
#endif

} // namespace Frame