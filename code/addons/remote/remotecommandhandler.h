#pragma once
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteCommandHandler
    
    Handles simple string commands (not XML) which are decoded into a
    Util::CommandLineArgs object.
    NOTE: Every command handler gets all received commands! One
    command handler class should handle several commands.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/commandlineargs.h"
#include "util/string.h"
#include "util/stringatom.h"
#include "threading/safequeue.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteCommandHandler : public Core::RefCounted
{
    __DeclareClass(RemoteCommandHandler);
public:
    /// constructor
    RemoteCommandHandler();
    /// destructor
    virtual ~RemoteCommandHandler();

protected:
    friend class RemoteControl;
    friend class RemoteControlProxy;

    /// put command
    void PutCommand(const Util::CommandLineArgs& cmd);
    /// handle pending commands
    void HandlePendingCommands();
    /// handle command
    virtual void HandleCommand(const Util::CommandLineArgs& cmd);

    Threading::SafeQueue<Util::CommandLineArgs> pendingCommands;       
    Util::Array<Util::CommandLineArgs> curWorkCommands;
};

} // namespace Remote
//------------------------------------------------------------------------------