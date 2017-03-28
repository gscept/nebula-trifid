//------------------------------------------------------------------------------
//  gridaddon.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "cloudsaddon.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "cloudsrtplugin.h"

namespace Clouds
{
__ImplementClass(Clouds::CloudsAddon, 'CLAD', Core::RefCounted);
__ImplementSingleton(Clouds::CloudsAddon);

//------------------------------------------------------------------------------
/**
*/
CloudsAddon::CloudsAddon()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
CloudsAddon::~CloudsAddon()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsAddon::Setup()
{
	// register render plugin
	RenderModules::RTPluginRegistry::Instance()->RegisterRTPlugin(&CloudsRTPlugin::RTTI);
	this->plugin = RenderModules::RTPluginRegistry::Instance()->GetPluginByRTTI<CloudsRTPlugin>(&CloudsRTPlugin::RTTI);
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsAddon::Discard()
{
	this->plugin = 0;

	// deregister plugin
	RenderModules::RTPluginRegistry::Instance()->UnregisterRTPlugin(&CloudsRTPlugin::RTTI);
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsAddon::SetVisible(bool b)
{	
	this->plugin->SetVisible(b);
}

//------------------------------------------------------------------------------
/**
*/
void
CloudsAddon::SetCloudSettings(const CloudsRTPlugin::CloudSettings& settings, IndexT index)
{
	this->plugin->SetCloudSettings(settings, index);
}

} // namespace Grid