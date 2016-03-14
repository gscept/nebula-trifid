//------------------------------------------------------------------------------
//  uirtplugin.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "uiserver.h"
#include "uirtplugin.h"
#include "frame/framebatch.h"
#include "uifeatureunit.h"


namespace UI
{
__ImplementClass(UI::UiRTPlugin, 'UIRT', RenderModules::RTPlugin);

using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
UiRTPlugin::UiRTPlugin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
UiRTPlugin::~UiRTPlugin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
UiRTPlugin::OnRegister()
{
	this->uiServer = UiServer::Instance();
}

//------------------------------------------------------------------------------
/**
*/
void
UiRTPlugin::OnUnregister()
{
	this->uiServer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
UiRTPlugin::OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch)
{
	n_assert(this->uiServer.isvalid());
	if (CoreGraphics::FrameBatchType::UI == frameBatch->GetType())
	{				
		this->uiServer->Render(frameBatch);	
		UiFeatureUnit::Instance()->RenderPlugins(frameBatch);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
UiRTPlugin::OnWindowResized( SizeT width, SizeT height )
{
	n_assert(this->uiServer.isvalid());
	this->uiServer->Resize(width, height);	
}
} // namespace RenderModules