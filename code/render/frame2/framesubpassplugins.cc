//------------------------------------------------------------------------------
// framesubpassplugins.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framesubpassplugins.h"

namespace Frame2
{

__ImplementClass(Frame2::FrameSubpassPlugins, 'FSPL', Frame2::FrameOp);
//------------------------------------------------------------------------------
/**
*/
FrameSubpassPlugins::FrameSubpassPlugins() :
	pluginRegistry(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
FrameSubpassPlugins::~FrameSubpassPlugins()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
FrameSubpassPlugins::Setup()
{
	n_assert(!this->pluginRegistry.isvalid());
	this->pluginRegistry = RenderModules::RTPluginRegistry::Instance();
}

//------------------------------------------------------------------------------
/**
*/
void
FrameSubpassPlugins::Run(const IndexT frameIndex)
{
	this->pluginRegistry->OnRender(this->pluginFilter);
}

} // namespace Frame2