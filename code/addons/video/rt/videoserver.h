#pragma once
//------------------------------------------------------------------------------
/**
    @class Video::VideoServer
    
    The VideoServer is the central object of the Video subsystem. 

    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/singleton.h"
#include "video/theora/theoravideoserver.h"
namespace Video
{
class VideoServer : public Video::TheoraVideoServer
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
//------------------------------------------------------------------------------