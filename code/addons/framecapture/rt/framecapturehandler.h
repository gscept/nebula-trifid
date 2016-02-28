#pragma once
//------------------------------------------------------------------------------
/**
    @class FrameCapture::FrameCaptureHandler

    Handles frame capture messages in the graphics thread.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file	
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"
#include "framecapture/framecaptureprotocol.h"

namespace FrameCapture
{
class FrameCaptureHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(FrameCaptureHandler);
public:
    /// Constructor
    FrameCaptureHandler();
    /// Destructor
    virtual ~FrameCaptureHandler();

    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);
    /// do per-frame work
    virtual void DoWork();

private:
    /// on start capture frame
    void OnStartCaptureFrame();
    /// on stop capture frame
    void OnStopCaptureFrame();
    // on save screenshot
    void OnSaveScreenShot();
};
} // namespace FrameCapture