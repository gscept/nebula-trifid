#pragma once
//------------------------------------------------------------------------------
/**
    @class FX::FXRTPlugin

    Setup render-thread environment for the FX system

    (C) 2009 Radon Labs GmbH
*/

#include "rendermodules/rt/rtplugin.h"
#include "fx/rt/fxhandler.h"
#include "fx/rt/fxserver.h"

namespace FX
{
class FXRTPlugin : public RenderModules::RTPlugin
{
__DeclareClass(FXRTPlugin);

public:
    /// Constructor
    FXRTPlugin();
    /// Destructor
    virtual ~FXRTPlugin();

    /// Called when registered on the render-thread side
    virtual void OnRegister();
    /// Called when unregistered from the render-thread side
    virtual void OnUnregister();
    /// Called after rendering
    virtual void OnRenderBefore(IndexT frameId, Timing::Time time);

private:
    Ptr<FxServer> fxServer;
    Ptr<FxHandler> fxHandler;

};
} // namespace FX