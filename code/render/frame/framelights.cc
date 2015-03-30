//------------------------------------------------------------------------------
//  framelights.cc
//  (C) 2011 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framelights.h"
#include "lighting/lightserver.h"
#include "coregraphics/renderdevice.h"

namespace Frame
{
__ImplementClass(Frame::FrameLights, 'FRML', Core::RefCounted);

using namespace Lighting;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FrameLights::FrameLights()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameLights::~FrameLights()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameLights::Discard()
{
	FramePassBase::Discard();
	_discard_timer(debugTimer);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameLights::Render()
{
	_start_timer(this->debugTimer);

	n_assert(this->renderTarget.isvalid() || this->multipleRenderTarget.isvalid());
	const Ptr<RenderDevice>& renderDevice = RenderDevice::Instance();
	const Ptr<LightServer>& lightServer = LightServer::Instance(); 

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
		// the multiple render target handles clear colors and such ly, so no real need to do it here aswell...
	}

	// apply those shader variables!
	IndexT varIndex;
	for (varIndex = 0; varIndex < this->shaderVariables.Size(); varIndex++)
	{
		this->shaderVariables[varIndex]->Apply();
	}

	if (this->renderTarget.isvalid())
	{
		n_assert(!this->multipleRenderTarget.isvalid());
		renderDevice->BeginPass(this->renderTarget, NULL);
	}
	else if (this->multipleRenderTarget.isvalid())
	{
		n_assert(!this->renderTarget.isvalid());
		renderDevice->BeginPass(this->multipleRenderTarget, NULL);
	}

	// render those darn lights!
	LightServer::Instance()->RenderLights();

	renderDevice->SetPassShader(0);
	renderDevice->EndPass();

	_stop_timer(this->debugTimer);

}

//------------------------------------------------------------------------------
/**
*/
#if NEBULA3_ENABLE_PROFILING
void 
FrameLights::SetFrameLightPassDebugTimer(const Util::String& name)
{
	this->debugTimer = Debug::DebugTimer::Create();
	this->debugTimer->Setup(name);
}
#endif

} // namespace Frame