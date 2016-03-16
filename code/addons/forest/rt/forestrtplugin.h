//------------------------------------------------------------------------------
/**
    @class Forest::ForestRTPlugin
    
    Setup render-thread environment for the forest subsystem

    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/

#include "forest/rt/foresthandler.h"
#include "forest/rt/forestserver.h"
#include "rendermodules/rt/rtplugin.h"

namespace Forest
{
class ForestRTPlugin : public RenderModules::RTPlugin
{
__DeclareClass(ForestRTPlugin);

public:
    /// Constructor
    ForestRTPlugin();
    /// Destructor
    virtual ~ForestRTPlugin();
    
    /// Called when registered on the render-thread side
    virtual void OnRegister();
    /// Called when unregistered from the render-thread side
    virtual void OnUnregister();
    /// Called before rendering
    virtual void OnRenderBefore(IndexT frameId, Timing::Time time);
    /// called after rendering entities
    virtual void OnRenderAfter(IndexT frameId, Timing::Time time);

private:
    
    Ptr<ForestServer> forestServer;
    Ptr<ForestHandler> forestHandler;

};
} // namespace Forest