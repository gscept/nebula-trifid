#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaPlugin
    
    Front-end class of the Nebula3 Maya plugin.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "mayamain/mayanebula3runtime.h"
#include "mayamain/mayalogger.h"
#include "mayamain/mayadirectoryregistry.h"
#include "mayamain/mayacommandregistry.h"
#include "mayashaders/mayashaderregistry.h"
#include "mayacore/mayavariableregistry.h"
#include "mayacore/mayamaterialgroupregistry.h"
#include <maya/MObject.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaPlugin : public Core::RefCounted
{
    __DeclareClass(MayaPlugin);
    __DeclareSingleton(MayaPlugin);
public:
    /// constructor
    MayaPlugin();
    /// destructor
    virtual ~MayaPlugin();
    
    /// setup the plugin
    bool Setup(MObject pluginObject);
    /// discard the plugin
    void Discard();
    /// return true if object is valid
    bool IsValid() const;

private:
    /// start the MEL environment
    bool MELStartup();
    /// shutdown the MEL environment
    void MELShutdown();

    MObject mayaPluginObject;
    Ptr<MayaNebula3Runtime> mayaNebula3Runtime;
    Ptr<MayaLogger> mayaLogger;
    Ptr<MayaDirectoryRegistry> mayaDirectoryRegistry;
    Ptr<MayaCommandRegistry> mayaCommandRegistry;
    Ptr<MayaShaderRegistry> mayaShaderRegistry;
    Ptr<MayaVariableRegistry> mayaVariableRegistry;
    Ptr<MayaMaterialGroupRegistry> mayaMaterialGroupRegistry;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaPlugin::IsValid() const
{
    return this->isValid;
}

} // namespace Maya
//------------------------------------------------------------------------------

    