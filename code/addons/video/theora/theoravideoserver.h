#pragma once
//------------------------------------------------------------------------------
/**
    @class Video::TheoraVideoServer
    
    The VideoServer based on the theora video library
        
    (C) 2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "video/base/videoserverbase.h"
#include "theoravideoplayer.h"

//------------------------------------------------------------------------------
namespace Video
{
class TheoraVideoServer : public Base::VideoServerBase
{
	__DeclareClass(TheoraVideoServer);
public: 

    /// constructor
	TheoraVideoServer();
    /// destructor
	virtual ~TheoraVideoServer();

    /// close the video server
    virtual void Close();

    /// call on frame
    virtual void OnFrame(Timing::Time time);
    /// called before rendering
    virtual void OnRenderBefore(Timing::Time time);

    /// play a video
    virtual void StartVideo(const Util::StringAtom& resName, 
                            const Math::float2& upperLeft,
                            const Math::float2& upperRight,
                            const Math::float2& lowerLeft,
                            const Math::float2& lowerRight,
                            bool fromFile,
                            bool autoDelete,
                            bool loop);
    /// stop a video
    virtual void StopVideo(const Util::StringAtom& resName,
                           bool del);
    /// pause a video
    virtual void PauseVideo(const Util::StringAtom& resName);
    /// resume a video
    virtual void ResumeVideo(const Util::StringAtom& resName);
    /// delete a video resource
    virtual bool DeleteVideo(const Util::StringAtom& resName);

    /// is video playing
    virtual bool IsVideoPlaying(const Util::StringAtom& resName);
    /// is video pausing
    virtual bool IsVideoPausing(const Util::StringAtom& resName);

	///
	virtual void RenderBatch();

protected:
    /// setup video system
    virtual void SetupVideoSystem();

private:
	Util::Dictionary<Util::StringAtom, Ptr<Video::TheoraVideoPlayer>> players;

};

} // namespace Video
//------------------------------------------------------------------------------