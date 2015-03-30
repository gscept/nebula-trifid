//------------------------------------------------------------------------------
//  frametext.cc
//  (C) 2011 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/frametext.h"
#include "coregraphics/textrenderer.h"
#include "coregraphics/renderdevice.h"

namespace Frame
{
__ImplementClass(Frame::FrameText, 'FRMT', Core::RefCounted);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FrameText::FrameText()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameText::~FrameText()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameText::Discard()
{
	FramePassBase::Discard();
	_discard_timer(debugTimer);
}

//------------------------------------------------------------------------------
/**
*/
void 
FrameText::Render()
{
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
		// FIXME!???
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

	TextRenderer::Instance()->DrawTextElements();
	renderDevice->EndPass();
}

//------------------------------------------------------------------------------
/**
*/
#if NEBULA3_ENABLE_PROFILING
void 
FrameText::SetFrameTextPassDebugTimer(const Util::String& name)
{
	this->debugTimer = Debug::DebugTimer::Create();
	this->debugTimer->Setup(name);
}
#endif
} // namespace Frame