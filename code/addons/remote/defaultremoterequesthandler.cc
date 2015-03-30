//------------------------------------------------------------------------------
//  defaultremoterequesthandler.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "core/coreserver.h"
#include "remote/defaultremoterequesthandler.h"
#include "timing/calendartime.h"
#include "remote/remotecontrol.h"

namespace Remote
{
__ImplementClass(Remote::DefaultRemoteRequestHandler, 'DRRH', Remote::RemoteRequestHandler);

using namespace Core;
using namespace Util;
using namespace Timing;

//------------------------------------------------------------------------------
/**
*/
void
DefaultRemoteRequestHandler::HandleRequest(const Ptr<RemoteRequest>& request)
{
    //
    // implement request handle here!!!
    //
}

} // namespace Remote