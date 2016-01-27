//------------------------------------------------------------------------------
//  videohandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "video/rt/videohandler.h"
#include "video/videoprotocol.h"
#include "video/rt/videoserver.h"

namespace Video
{
__ImplementClass(VideoHandler, 'VIDH', Interface::InterfaceHandlerBase);

//------------------------------------------------------------------------------
/**
*/
VideoHandler::VideoHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
VideoHandler::~VideoHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
VideoHandler::HandleMessage(const Ptr<Messaging::Message>& msg)
{
    n_assert(msg.isvalid());
    if (msg->CheckId(StopVideo::Id))
    {
        this->OnStopVideo(msg.cast<StopVideo>());
    }
    else if (msg->CheckId(PlayVideo::Id))
    {
        this->OnPlayVideo(msg.cast<PlayVideo>());
    }
    else if (msg->CheckId(PauseVideo::Id))
    {
        this->OnPauseVideo(msg.cast<PauseVideo>());
    }
    else if (msg->CheckId(ResumeVideo::Id))
    {
        this->OnResumeVideo(msg.cast<ResumeVideo>());
    }
    else if (msg->CheckId(IsVideoPlaying::Id))
    {
        this->OnIsVideoPlaying(msg.cast<IsVideoPlaying>());
    }
    else if (msg->CheckId(DeleteVideo::Id))
    {
        this->OnDeleteVideo(msg.cast<DeleteVideo>());
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
*/
void
VideoHandler::OnPauseVideo(const Ptr<PauseVideo>& msg)
{
    VideoServer::Instance()->PauseVideo(msg->GetResourceName());
}

//------------------------------------------------------------------------------
/**
*/
void
VideoHandler::OnResumeVideo(const Ptr<ResumeVideo>& msg)
{
    VideoServer::Instance()->ResumeVideo(msg->GetResourceName());
}

//------------------------------------------------------------------------------
/**
*/
void 
VideoHandler::OnIsVideoPlaying(const Ptr<IsVideoPlaying>& msg)
{
    msg->SetPlaying(VideoServer::Instance()->IsVideoPlaying(msg->GetResourceName()));
}

//------------------------------------------------------------------------------
/**
*/
void
VideoHandler::OnStopVideo(const Ptr<StopVideo>& msg)
{	
    VideoServer::Instance()->StopVideo(msg->GetResourceName(), msg->GetDeleteVideo());
}

//------------------------------------------------------------------------------
/**
*/
void
VideoHandler::OnPlayVideo(const Ptr<PlayVideo>& msg)
{
    VideoServer::Instance()->StartVideo(msg->GetResourceName(), 
                                        msg->GetUpperLeftCorner(), 
                                        msg->GetUpperRightCorner(),
                                        msg->GetLowerLeftCorner(),
                                        msg->GetLowerRightCorner(),
                                        msg->GetFromFile(),
                                        msg->GetAutoDelete(),
                                        msg->GetLoop());
}

//------------------------------------------------------------------------------
/**
*/
void
VideoHandler::OnDeleteVideo(const Ptr<DeleteVideo>& msg)
{
    VideoServer::Instance()->DeleteVideo(msg->GetResourceName());
}
} // namespace Video
