//------------------------------------------------------------------------------
//  mayacommandregistry.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayacommandregistry.h"
#include "mayacommands/mayadirectoryregistrycmd.h"
#include "mayacommands/mayaattrutilcmd.h"
#include "mayacommands/mayaexportscenecmd.h"
#include "mayalegacy/mayaenvcmd.h"
#include "mayalegacy/mayaqueryshadercmd.h"
#include "mayalegacy/mayaremotecontrolcmd.h"
#include "mayamain/mayalogger.h"

#define MNoPluginEntry
#define MNoVersionString
#include <maya/MFnPlugin.h>

namespace Maya
{
__ImplementClass(Maya::MayaCommandRegistry, 'MCRG', Core::RefCounted);
__ImplementSingleton(Maya::MayaCommandRegistry);

//------------------------------------------------------------------------------
/**
*/
MayaCommandRegistry::MayaCommandRegistry() :
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaCommandRegistry::~MayaCommandRegistry()
{
    n_assert(!this->IsValid());
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCommandRegistry::Setup(MFnPlugin& fnPlugin)
{
    n_assert(!this->IsValid());

    // register MEL commands
    fnPlugin.registerCommand("rlDirectoryRegistryCmd", MayaDirectoryRegistryCmd::creator, MayaDirectoryRegistryCmd::getSyntax);
    fnPlugin.registerCommand("rlAttrUtilCmd", MayaAttrUtilCmd::creator, MayaAttrUtilCmd::getSyntax);
    fnPlugin.registerCommand("exportScene", MayaExportSceneCmd::creator, MayaExportSceneCmd::getSyntax);

    // register legacy commands
    fnPlugin.registerCommand("rlEnv", MayaEnvCmd::creator, MayaEnvCmd::getSyntax);
    fnPlugin.registerCommand("queryShader", MayaQueryShaderCmd::creator, MayaQueryShaderCmd::getSyntax);
    fnPlugin.registerCommand("remoteControl", MayaRemoteControlCmd::creator, MayaRemoteControlCmd::getSyntax);

    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaCommandRegistry::Discard(MFnPlugin& fnPlugin)
{
    n_assert(this->IsValid());

    // unregister legacy commands
    fnPlugin.deregisterCommand("remoteControl");
    fnPlugin.deregisterCommand("queryShader");
    fnPlugin.deregisterCommand("rlEnv");

    // unregister MEL commands
    fnPlugin.deregisterCommand("exportScene");
    fnPlugin.deregisterCommand("rlAttrUtilCmd");
    fnPlugin.deregisterCommand("rlDirectoryRegistryCmd");

    this->isValid = false;
}

} // namespace Maya
