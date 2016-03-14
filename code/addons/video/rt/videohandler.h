#pragma once
//------------------------------------------------------------------------------
/**
    @class Video::VideoHandler
    
    Handles the video stuff
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "interface/interfacehandlerbase.h"
#include "messaging/message.h"

//------------------------------------------------------------------------------
namespace Video
{
class PlayVideo;
class StopVideo;
class PauseVideo;
class ResumeVideo;
class IsVideoPlaying;
class DeleteVideo;

class VideoHandler : public Interface::InterfaceHandlerBase
{
    __DeclareClass(VideoHandler);

public:
    /// constructor
    VideoHandler();
    /// destructor
    virtual ~VideoHandler();    
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);

private:
    /// handle video start
    void OnPlayVideo(const Ptr<PlayVideo>& msg);
    /// handle video stop
    void OnStopVideo(const Ptr<StopVideo>& msg);
    /// handle pause video
    void OnPauseVideo(const Ptr<PauseVideo>& msg);
    /// handle video resume
    void OnResumeVideo(const Ptr<ResumeVideo>& msg);
    /// handle delete video
    void OnDeleteVideo(const Ptr<DeleteVideo>& msg);
    /// handle is video playing
    void OnIsVideoPlaying(const Ptr<IsVideoPlaying>& msg);
};

} // namespace Video
//------------------------------------------------------------------------------