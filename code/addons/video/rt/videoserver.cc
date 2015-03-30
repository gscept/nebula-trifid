//------------------------------------------------------------------------------
//  videoserver.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "video/rt/videoserver.h"

namespace Video
{
#if __WIN32__
__ImplementClass(Video::VideoServer, 'VIDS', Win32::Win32VideoServer);
#else
#error "VideoServer class not implemented on this platform!"
#endif
__ImplementSingleton(Video::VideoServer);

//------------------------------------------------------------------------------
/**
*/
VideoServer::VideoServer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
VideoServer::~VideoServer()
{
    __DestructSingleton;
}

} // namespace Video
