//------------------------------------------------------------------------------
//  gridaddon.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "gridaddon.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "gridrtplugin.h"

namespace Grid
{
__ImplementClass(Grid::GridAddon, 'GRAD', Core::RefCounted);
__ImplementSingleton(Grid::GridAddon);

//------------------------------------------------------------------------------
/**
*/
GridAddon::GridAddon()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
GridAddon::~GridAddon()
{
	__DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
GridAddon::Setup()
{
	// register render plugin
	RenderModules::RTPluginRegistry::Instance()->RegisterRTPlugin(&GridRTPlugin::RTTI);
	this->plugin = RenderModules::RTPluginRegistry::Instance()->GetPluginByRTTI<GridRTPlugin>(&GridRTPlugin::RTTI);
}

//------------------------------------------------------------------------------
/**
*/
void
GridAddon::Discard()
{
	this->plugin = 0;

	// deregister plugin
	RenderModules::RTPluginRegistry::Instance()->UnregisterRTPlugin(&GridRTPlugin::RTTI);
}

//------------------------------------------------------------------------------
/**
*/
void
GridAddon::SetVisible(bool b)
{	
	this->plugin->SetVisible(b);
}

//------------------------------------------------------------------------------
/**
*/
void
GridAddon::SetGridSize(int size)
{
	this->plugin->SetGridSize(size);
}

//------------------------------------------------------------------------------
/**
*/
void
GridAddon::SetCellSize(float size)
{
	this->plugin->SetCellSize(size);
}

} // namespace Grid