#pragma once
//------------------------------------------------------------------------------
/**
    @class Video::VideoServer
    
    The VideoServer is the central object of the Video subsystem. 

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#if __WIN32__
#include "video/win32/win32videoserver.h"
namespace Video
{
class VideoServer : public Win32::Win32VideoServer
{
    __DeclareClass(VideoServer);
    __DeclareSingleton(VideoServer);
public:
    /// constructor
    VideoServer();
    /// destructor
    virtual ~VideoServer();
};
} // namespace Video
#else
#error "VideoServer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------