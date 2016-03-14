#pragma once
//------------------------------------------------------------------------------
/**
    @class FrameCapture::FrameCaptureRTPlugin

    Setup render-thread environment for the frame capture subsystem

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "rendermodules/rt/rtplugin.h"

//------------------------------------------------------------------------------
namespace FrameCapture
{
class FrameCaptureServer;
class FrameCaptureHandler;
class FrameCapturePageHandler;

class FrameCaptureRTPlugin : public RenderModules::RTPlugin
{
    __DeclareClass(FrameCaptureRTPlugin);
public:
    /// Constructor
    FrameCaptureRTPlugin();
    /// Destructor
    virtual ~FrameCaptureRTPlugin();
    /// called when plugin is registered on the render-thread side
    virtual void OnRegister();
    /// called when plugin is unregistered on the render-thread side
    virtual void OnUnregister();
private:
    Ptr<FrameCaptureServer> frameCaptureServer;
    Ptr<FrameCaptureHandler> frameCaptureHandler;
    Ptr<FrameCapturePageHandler> httpHandler;
};
} // namespace FrameCapture