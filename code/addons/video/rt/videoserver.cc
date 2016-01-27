//------------------------------------------------------------------------------
//  videoserver.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "video/rt/videoserver.h"

namespace Video
{
__ImplementClass(Video::VideoServer, 'VIDS', Video::TheoraVideoServer);
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
