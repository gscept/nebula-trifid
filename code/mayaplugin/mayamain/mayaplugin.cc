//------------------------------------------------------------------------------
//  mayaplugin.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayamain/mayaplugin.h"
#include "mayautil/mayatype.h"
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

namespace Maya
{
__ImplementClass(Maya::MayaPlugin, 'MPGN', Core::RefCounted);
__ImplementSingleton(Maya::MayaPlugin);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaPlugin::MayaPlugin() :
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaPlugin::~MayaPlugin()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
    Setup the MayaPlugin object. The provided MObject parameter is the
    object from the initializePlugin() function.
*/
bool
MayaPlugin::Setup(MObject pluginObject)
{
    n_assert(!this->IsValid());
    this->isValid = true;

    this->mayaPluginObject = pluginObject;
    MFnPlugin fnPlugin(pluginObject, "gscept", "3.0");

    // setup singletons
    this->mayaNebula3Runtime = MayaNebula3Runtime::Create();
    this->mayaNebula3Runtime->Setup();

    this->mayaLogger = MayaLogger::Create();
    this->mayaLogger->Setup();

    this->mayaDirectoryRegistry = MayaDirectoryRegistry::Create();
    this->mayaDirectoryRegistry->Setup();

	this->mayaCommandRegistry = MayaCommandRegistry::Create();
	this->mayaCommandRegistry->Setup(fnPlugin);

	/*
    this->mayaVariableRegistry = MayaVariableRegistry::Create();
    this->mayaVariableRegistry->Setup();



    this->mayaShaderRegistry = MayaShaderRegistry::Create();
    this->mayaShaderRegistry->Setup();

    this->mayaMaterialGroupRegistry = MayaMaterialGroupRegistry::Create();
    this->mayaMaterialGroupRegistry->Setup();
	*/

    // execute the MEL startup function
    if (!this->MELStartup())
    {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaPlugin::Discard()
{
    n_assert(this->IsValid());
    MFnPlugin fnPlugin(this->mayaPluginObject);

    // call the MEL shutdown function
    this->MELShutdown();

    // discard singletons
	/*
    this->mayaMaterialGroupRegistry->Discard();
    this->mayaMaterialGroupRegistry = 0;

    this->mayaShaderRegistry->Discard();
    this->mayaShaderRegistry = 0;



    this->mayaVariableRegistry->Discard();
    this->mayaVariableRegistry = 0;
	*/

	this->mayaCommandRegistry->Discard(fnPlugin);
	this->mayaCommandRegistry = 0;

    this->mayaDirectoryRegistry->Discard();
    this->mayaDirectoryRegistry = 0;

    this->mayaLogger->Discard();
    this->mayaLogger = 0;

    this->mayaNebula3Runtime->Discard();
    this->mayaNebula3Runtime = 0;

    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
bool
MayaPlugin::MELStartup()
{
    MStatus status;
    String projPath = MayaDirectoryRegistry::Instance()->GetProjectPath();

    // load the rlstartup.mel script
    String startupScript;
    startupScript.Format("%s/data/maya/pluginscripts/rlnebula3.mel", projPath.AsCharPtr());
    status = MGlobal::sourceFile(MayaType::Cast<String,MString>(startupScript));
    if (MS::kSuccess != status)
    {
        MayaLogger::Instance()->Error("Error loading startup script '%s'!\n", startupScript.AsCharPtr());
        return false;
    }

    // execute the rlStartup3 MEL function
    String cmdString;
    cmdString.Format("rlStartup3(\"%s\");", projPath.AsCharPtr());
    status = MGlobal::executeCommand(MayaType::Cast<String,MString>(cmdString), true, false);
    if (MS::kSuccess != status)
    {
        MayaLogger::Instance()->Error("Error executing the rlStartup3 command!\n");
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MayaPlugin::MELShutdown()
{
    // execute the rlShutdown MEL function
    MGlobal::executeCommand("rlShutdown3;");
}

} // namespace Maya
