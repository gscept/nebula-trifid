#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::VideoServerBase
    
    The VideoServer is the central object of the Video subsystem. 
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Base
{
class VideoServerBase : public Core::RefCounted
{
    __DeclareClass(VideoServerBase);
public:

    enum VideoState 
    {
        Stopped = 0,
        Playing,
        Paused,

        NumVideoStates,
        InvalidVideoState
    };

    /// constructor
    VideoServerBase();
    /// destructor
    virtual ~VideoServerBase();

    /// open the video server
    virtual void Open();
    /// close the video server
    virtual void Close();
    /// return true if open
    bool IsOpen() const;

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

protected:
    /// setup video system
    virtual void SetupVideoSystem();

private:
    bool isOpen;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
VideoServerBase::IsOpen() const
{
    return this->isOpen;
}
} // namespace Base
//------------------------------------------------------------------------------