//------------------------------------------------------------------------------
//  displaysettings.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/displaysettings.h"

namespace Graphics
{

using namespace Util;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
DisplaySettings::DisplaySettings() :
    adapter(Adapter::Primary),
    displayMode(0, 0, 1024, 768, PixelFormat::R8G8B8X8),
    antiAliasQuality(AntiAliasQuality::None),
    fullscreen(false),
    modeSwitchEnabled(true),
    tripleBufferingEnabled(false),
    alwaysOnTop(false),
    verticalSync(false),
    decorated(true),
    resizable(true),
    iconName("NebulaIcon"),
    windowTitle("Nebula Trifid Application Window"),
	embedded(false)
{
    // empty
}

} // namespace Graphics
