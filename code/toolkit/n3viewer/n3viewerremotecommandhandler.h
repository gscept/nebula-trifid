#pragma once
//------------------------------------------------------------------------------
/**
    @class Toolkit::N3ViewerRemoteCommandHandler
    
    Remote command handler for N3 viewer.
    
    (C) 2010 Radon Labs GmbH
*/
#include "remote/remotecommandhandler.h"

//------------------------------------------------------------------------------
namespace Toolkit
{
class N3ViewerRemoteCommandHandler : public Remote::RemoteCommandHandler
{
    __DeclareClass(N3ViewerRemoteCommandHandler);
protected:
    /// handle command
    virtual void HandleCommand(const Util::CommandLineArgs& cmd);
};

} // namespace Toolkit