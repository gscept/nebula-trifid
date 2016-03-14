//------------------------------------------------------------------------------
//  remotecommandhandler.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "remote/remotecommandhandler.h"

namespace Remote
{
__ImplementClass(Remote::RemoteCommandHandler, 'COHN', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
RemoteCommandHandler::RemoteCommandHandler()
{
    this->pendingCommands.SetSignalOnEnqueueEnabled(false);
}

//------------------------------------------------------------------------------
/**
*/
RemoteCommandHandler::~RemoteCommandHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Put a remote request into the request handlers message queue. This method
    is meant to be called from another thread.
*/
void
RemoteCommandHandler::PutCommand(const Util::CommandLineArgs& cmd)
{
    this->pendingCommands.Enqueue(cmd);
}

//------------------------------------------------------------------------------
/**
    Handle all pending commands in the pending queue. This method
    must be called frequently from the thread which created this
    command handler.
*/
void
RemoteCommandHandler::HandlePendingCommands()
{
    this->pendingCommands.DequeueAll(this->curWorkCommands);
    IndexT i;
    for (i = 0; i < this->curWorkCommands.Size(); i++)
    {
        this->HandleCommand(this->curWorkCommands[i]);
    }
    this->curWorkCommands.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
RemoteCommandHandler::HandleCommand(const Util::CommandLineArgs& cmd)
{
    // implement in subclass!  
}

} // namespace Remote
