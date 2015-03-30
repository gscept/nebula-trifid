//------------------------------------------------------------------------------
//  mayapluginmain.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayamain/mayaplugin.h"
#include <maya/mstatus.h>

// global smart pointer to keep the plugin object alive
static Ptr<Maya::MayaPlugin> MayaPluginPtr;

//------------------------------------------------------------------------------
/**
    This method is called by Maya after the plugin DLL has been loaded to
    setup the plugin.
*/
PLUGIN_EXPORT MStatus
initializePlugin(MObject pluginObject)
{
    // setup the Nebula3 MayaPlugin object
    n_assert(!MayaPluginPtr.isvalid());
    MayaPluginPtr = Maya::MayaPlugin::Create();
    if (MayaPluginPtr->Setup(pluginObject))
    {
        return MS::kSuccess;
    }
    else
    {
        return MS::kFailure;
    }
}

//------------------------------------------------------------------------------
/**
    This method is called by Maya when the plugin DLL is going to be 
    unloaded.
*/
PLUGIN_EXPORT MStatus
uninitializePlugin(MObject pluginObject)
{
    // discard the Nebula3 plugin object
    n_assert(MayaPluginPtr.isvalid());
    MayaPluginPtr->Discard();
    return MS::kSuccess;
}
