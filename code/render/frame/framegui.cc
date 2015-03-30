//------------------------------------------------------------------------------
//  framegui.cc
//  (C) 2011 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "frame/framegui.h"
#include "coregraphics/textrenderer.h"
#include "coregraphics/renderdevice.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "sui/rt/suirtplugin.h"


namespace Frame
{
__ImplementClass(Frame::FrameGUI, 'FRMG', Core::RefCounted);

using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
FrameGUI::FrameGUI()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameGUI::~FrameGUI()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameGUI::Discard()
{	
	FramePassBase::Discard();
	_discard_timer(debugTimer);
}

//------------------------------------------------------------------------------
/**
*/
void
FrameGUI::Render()
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

	RenderModules::RTPluginRegistry* rtPluginRegistry = RenderModules::RTPluginRegistry::Instance();
	// FIXME HAX
	if (Core::Factory::Instance()->ClassExists('SRTP'))
	{
	IndexT pluginIndex = rtPluginRegistry->FindPlugin(Core::Factory::Instance()->GetClassRtti(Util::FourCC("SRTP")));//&SUI::SUIRTPlugin::RTTI);
	if (pluginIndex != InvalidIndex)
	{
		Ptr<SUI::SUIRTPlugin> suiPlugin = rtPluginRegistry->GetRTPlugins()[pluginIndex].downcast<SUI::SUIRTPlugin>();	
		suiPlugin->OnRenderFrame();
	}
	}

	
	//rtPluginRegistry->OnRenderFrame();

	renderDevice->EndPass();

	_stop_timer(this->debugTimer);
}	


//------------------------------------------------------------------------------
/**
*/
#if NEBULA3_ENABLE_PROFILING
void 
FrameGUI::SetFrameGUIPassDebugTimer(const Util::String& name)
{
	this->debugTimer = Debug::DebugTimer::Create();
	this->debugTimer->Setup(name);
}
#endif

} // namespace Frame