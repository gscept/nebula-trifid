//------------------------------------------------------------------------------
//  minimapplugin.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "minimapplugin.h"
#include "resources/resourceid.h"
#include "resources/resourcemanager.h"
#include "minimapmanager.h"

using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Math;
using namespace Models;

namespace Minimap
{

//------------------------------------------------------------------------------
/**
*/
MinimapPlugin::MinimapPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MinimapPlugin::~MinimapPlugin()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Rocket::Core::EventListener*
MinimapPlugin::InstanceEventListener(const Rocket::Core::String& value, Rocket::Core::Element* element)
{
	if (element->GetTagName() == "minimap")
	{
		return this;
	}
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::ProcessEvent(Rocket::Core::Event& event)
{
	if (event.GetType() == "click")
	{
		float x = event.GetParameter<float>("mouse_x", 0);
		float y = event.GetParameter<float>("mouse_y", 0);

	}

}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::Release()
{

}


} // namespace Minimap

