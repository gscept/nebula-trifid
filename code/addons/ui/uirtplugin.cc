//------------------------------------------------------------------------------
//  uirtplugin.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "uiserver.h"
#include "uirtplugin.h"
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
UiRTPlugin::OnRender(const Util::StringAtom& filter)
{
	n_assert(this->uiServer.isvalid());
	if (filter == "UI")
	{				
		this->uiServer->Render(filter);	
		UiFeatureUnit::Instance()->RenderPlugins(filter);
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
UiRTPlugin::OnWindowResized(IndexT windowId, SizeT width, SizeT height)
{
	n_assert(this->uiServer.isvalid());
	this->uiServer->Resize(width, height);	
}
} // namespace RenderModules