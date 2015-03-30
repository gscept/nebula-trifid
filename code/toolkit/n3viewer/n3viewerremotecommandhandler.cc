//------------------------------------------------------------------------------
//  n3viewerremotecommandhandler.cc
//  (C) 2010 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "n3viewerremotecommandhandler.h"
#include "n3viewerapp.h"

namespace Toolkit
{
__ImplementClass(Toolkit::N3ViewerRemoteCommandHandler, 'N3VC', Remote::RemoteCommandHandler);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
void
N3ViewerRemoteCommandHandler::HandleCommand(const CommandLineArgs& cmd)
{
    if (cmd.GetCmdName() == "n3view")
    {
        N3ViewerApp::Instance()->DiscardModels();
        N3ViewerApp::Instance()->LoadModel(cmd.GetString("-model"));
    }
    else if (cmd.GetCmdName() == "loadSkin")
    {
        N3ViewerApp::Instance()->LoadSkin(cmd.GetString("-model"),
                                          cmd.GetString("-skin"));
    }
    else if (cmd.GetCmdName() == "removeSkin")
    {
        N3ViewerApp::Instance()->RemoveSkin(cmd.GetString("-model"),
                                            cmd.GetString("-skin"));
    }
    else if (cmd.GetCmdName() == "loadVariation")
    {
        N3ViewerApp::Instance()->LoadVariation(cmd.GetString("-model"),
                                               cmd.GetString("-var"));
    }
    else if (cmd.GetCmdName() == "loadAnimation")
    {
        N3ViewerApp::Instance()->LoadAnimation(cmd.GetString("-model"),
                                               cmd.GetString("-anim"));
    }

}

} // namespace Toolkit