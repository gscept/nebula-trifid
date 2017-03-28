#pragma once
//------------------------------------------------------------------------------
/**
    @class Commands::CreateEntityCommand

    (C) 2010 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "remote/remotecommandhandler.h"
#include "attr/attributecontainer.h"

//------------------------------------------------------------------------------
namespace Commands
{
class CreateEntityCommand : public Remote::RemoteCommandHandler
{
    __DeclareClass(CreateEntityCommand);
public:
    /// constructor
    CreateEntityCommand();
    /// destructor
    virtual ~CreateEntityCommand();

protected:
    /// handle command
    virtual void HandleCommand(const Util::CommandLineArgs& cmd);
    /// handle update of a default game entity
    void HandleDefaultEntity(const Util::String& category, const Util::String& tmplt, const Util::String& id, const Math::matrix44& entityTransform, const Attr::AttributeContainer& attrs);

    Util::String cmdName;
};

} // namespace Remote
//------------------------------------------------------------------------------