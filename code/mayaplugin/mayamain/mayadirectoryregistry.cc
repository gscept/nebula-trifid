//------------------------------------------------------------------------------
//  mayadirectoryregistry.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayadirectoryregistry.h"
#include "mayamain/mayalogger.h"
#include "system/win32/win32registry.h"
#include "io/assignregistry.h"

namespace Maya
{
__ImplementClass(Maya::MayaDirectoryRegistry, 'MDRG', Core::RefCounted);
__ImplementSingleton(Maya::MayaDirectoryRegistry);

using namespace Util;
using namespace IO;
using namespace System;

//------------------------------------------------------------------------------
/**
*/
MayaDirectoryRegistry::MayaDirectoryRegistry() :
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaDirectoryRegistry::~MayaDirectoryRegistry()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaDirectoryRegistry::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
    this->UpdateAssigns();
}

//------------------------------------------------------------------------------
/**
*/
void
MayaDirectoryRegistry::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaDirectoryRegistry::UpdateAssigns()
{
    n_assert(this->IsValid());

    // read Windows registry keys
    String projPath    = this->GetProjectPath();
	String toolkitPath = this->GetToolkitPath();

    // setup assigns
    AssignRegistry* assignReg = AssignRegistry::Instance();
    
    assignReg->SetAssign(Assign("proj", projPath));
	assignReg->SetAssign(Assign("toolkit", toolkitPath));

    assignReg->SetAssign(Assign("mb_src", "proj:work/gfxlib"));
    assignReg->SetAssign(Assign("tex_src", "proj:work/textures"));
    assignReg->SetAssign(Assign("model_dst", "export:models"));
    assignReg->SetAssign(Assign("mesh_dst", "export:meshes"));
    assignReg->SetAssign(Assign("anim_dst", "export:anims"));
    assignReg->SetAssign(Assign("tex_dst", "export:textures"));
    assignReg->SetAssign(Assign("physics_dst", "export:physics"));
}

//------------------------------------------------------------------------------
/**
    This method allows to switch to another project path. It will
    first update the Windows registry and then call UpdateAssigns.
*/
void
MayaDirectoryRegistry::SetProjectPath(const String& newProjectPath)
{
    n_assert(this->IsValid());
    n_assert(newProjectPath.IsValid());

    // first update the registry...
    Win32Registry::WriteString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir", newProjectPath);
    
    // then update assigns
    this->UpdateAssigns();
}

//------------------------------------------------------------------------------
/**
    Query the current project path.
*/
String
MayaDirectoryRegistry::GetProjectPath() const
{
    n_assert(this->IsValid());    
    String projPath;
    if (Win32Registry::Exists(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir"))
    {
        projPath = Win32Registry::ReadString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "workdir");
        projPath.ConvertBackslashes();
    }
    else
    {
        MayaLogger::Instance()->Warning("Registry key not found: CurrentUser/Software/gscept/ToolkitShared/workdir!\n");
        projPath = "home:";
    }
    return projPath;
}

//------------------------------------------------------------------------------
/**
    Switches the export root path to another location. Also updates
    the Windows registry.
*/
void
MayaDirectoryRegistry::SetExportPath(const String& newExportPath)
{
    n_assert(this->IsValid());
    n_assert(newExportPath.IsValid());

    // first update the registry...
    Win32Registry::WriteString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "export", newExportPath);

    // then update assigns
    this->UpdateAssigns();
}

//------------------------------------------------------------------------------
/**
    Query the current export path.
*/
String
MayaDirectoryRegistry::GetExportPath() const
{
    String exportPath;
    if (Win32Registry::Exists(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "export"))
    {
        exportPath = Win32Registry::ReadString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "export");
        exportPath.ConvertBackslashes();
    }
    else
    {
        MayaLogger::Instance()->Warning("Registry key not found: CurrentUser/Software/gscept/ToolkitShared/export!\n");
        exportPath = "proj:intermediate";
    }
    return exportPath;
}


//------------------------------------------------------------------------------
/**
*/
void 
MayaDirectoryRegistry::SetToolkitPath( const Util::String& newToolkitPath )
{
	n_assert(this->IsValid());
	n_assert(newToolkitPath.IsValid());

	// first update the registry...
	Win32Registry::WriteString(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "path", newToolkitPath);

	// then update assigns
	this->UpdateAssigns();
}

//------------------------------------------------------------------------------
/**
    Query the current toolkit path.
*/
String
MayaDirectoryRegistry::GetToolkitPath() const
{
    String toolkitPath;
    if (Win32Registry::Exists(Win32Registry::CurrentUser, "Software\\gscept\\ToolkitShared", "path"))
    {
        toolkitPath = Win32Registry::ReadString(Win32Registry::CurrentUser,"Software\\gscept\\ToolkitShared", "path");
        toolkitPath.ConvertBackslashes();
    }
    else
    {
        MayaLogger::Instance()->Warning("Registry key not found: CurrentUser/Software/gscept/ToolkitShared/path!\n");
        toolkitPath = "home:";
    }
    return toolkitPath;
}

//------------------------------------------------------------------------------
/**
    Get absolute path to temp: directory.
*/
String
MayaDirectoryRegistry::GetTempPath() const
{
    String tempPath = AssignRegistry::Instance()->ResolveAssignsInString("temp:");
    return tempPath;
}

} // namespace Maya