//------------------------------------------------------------------------------
// framecapturehandler.cc
// (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "framecapturehandler.h"
#include "framecapture/rt/framecaptureserver.h"

namespace FrameCapture
{
__ImplementClass(FrameCapture::FrameCaptureHandler,'FCHR',Interface::InterfaceHandlerBase);
//------------------------------------------------------------------------------
/**
    Constructor	
*/
FrameCaptureHandler::FrameCaptureHandler()
{
	// empty
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
FrameCaptureHandler::~FrameCaptureHandler()
{
	if (this->IsOpen())
	{
		this->Close();
	}
}

//------------------------------------------------------------------------------
/**
    Handle message	
*/
bool
FrameCaptureHandler::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    if (msg->CheckId(StartFrameCapture::Id))
    {
        this->OnStartCaptureFrame();
    }
    else if (msg->CheckId(StopFrameCapture::Id))
    {
        this->OnStopCaptureFrame();
    }
    else if (msg->CheckId(SaveScreenShot::Id))
    {
        this->OnSaveScreenShot();
    }
    else if (msg->CheckId(SetupFrameCapture::Id))
    {
        Ptr<SetupFrameCapture> setupMsg = msg.cast<SetupFrameCapture>();
        FrameCaptureServer::Instance()->SetBaseDirectory(setupMsg->GetBaseDirectory());
        FrameCaptureServer::Instance()->SetFrameTime(setupMsg->GetFrameTime());
        FrameCaptureServer::Instance()->SetCaptureFileFormat(setupMsg->GetCaptureFileFormat());
    }
    else if (msg->CheckId(GetFrameCaptureSetup::Id))
    {
        Ptr<GetFrameCaptureSetup> getMsg = msg.cast<GetFrameCaptureSetup>();
        getMsg->SetBaseDirectory(FrameCaptureServer::Instance()->GetBaseDirectory());
        getMsg->SetFrameTime(FrameCaptureServer::Instance()->GetFrameTime());
        getMsg->SetCaptureFileFormat(FrameCaptureServer::Instance()->GetCaptureFileFormat());
    }
    else
    {
        // unknown message
        return false;
    }
    // fallthrough: message was handled
    return true;
}

//------------------------------------------------------------------------------
/**
    Do per-frame work	
*/
void
FrameCaptureHandler::DoWork()
{
    FrameCaptureServer::Instance()->Trigger();
}

//------------------------------------------------------------------------------
/**
    Start frame capture	
*/
void
FrameCaptureHandler::OnStartCaptureFrame()
{
    FrameCaptureServer::Instance()->Start();
}

//------------------------------------------------------------------------------
/**
    Stop frame capture	
*/
void
FrameCaptureHandler::OnStopCaptureFrame()
{
    FrameCaptureServer::Instance()->Stop();
}

//------------------------------------------------------------------------------
/**
    Save screenshot
*/
void
FrameCaptureHandler::OnSaveScreenShot()
{
    FrameCaptureServer::Instance()->SaveScreenShot();
}
} // namespace FrameCapture