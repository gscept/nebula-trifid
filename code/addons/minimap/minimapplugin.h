#pragma once
//------------------------------------------------------------------------------
/**
    @class Minimap::MinimapPlugin
    
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "timing/time.h"
#include "frame/framebatch.h"
#include "ui/base/uiplugin.h"
#include "math/bbox.h"
#include "resources/managedtexture.h"

//------------------------------------------------------------------------------
namespace Minimap
{
class MinimapPlugin : public UI::UiPlugin
{
    __DeclareClass(MinimapPlugin);
public:
    /// constructor
    MinimapPlugin();
    /// destructor
    virtual ~MinimapPlugin();

    /// called when plugin is registered
    virtual void OnRegister();
    /// called when plugin is unregistered
    virtual void OnUnregister();
	
};

} // namespace Minimap