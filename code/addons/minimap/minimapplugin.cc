//------------------------------------------------------------------------------
//  minimapplugin.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "minimapplugin.h"
#include "resources/resourceid.h"
#include "resources/resourcemanager.h"

using namespace Util;
using namespace CoreGraphics;
using namespace Resources;
using namespace Math;
using namespace Models;

namespace Minimap
{
__ImplementClass(Minimap::MinimapPlugin, 'MMPG', UI::UiPlugin);


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
void
MinimapPlugin::OnRegister()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MinimapPlugin::OnUnregister()
{
	// empty
}

} // namespace Minimap

