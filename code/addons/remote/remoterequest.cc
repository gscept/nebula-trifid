//------------------------------------------------------------------------------
//  remoterequest.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remoterequest.h"

namespace Remote
{
__ImplementClass(Remote::RemoteRequest, 'RERQ', Messaging::Message);
__ImplementMsgId(RemoteRequest);

//------------------------------------------------------------------------------
/**
*/
RemoteRequest::RemoteRequest() :
    status(RemoteStatus::InvalidRemoteStatus)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RemoteRequest::~RemoteRequest()
{
    // empty
}

} // namespace Remote
