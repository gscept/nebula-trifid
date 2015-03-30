#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaDirectoryRegistry
    
    Manages the source and target directories for the Maya plugin.
    Directories are managed as Nebula3 path assigns.
    There are 3 main directories from which all other directories 
    are derived:

    toolkit:    the installation directory of the toolkit
    proj:       the current project directory
    export:     the root directory where files are exported to

    The toolkit: and proj: directory paths are kept in the Windows
    registry, the export: directory path is by default proj:export
    and can be overwritten after the plugin is initialized.

    The following path assigns are derived from the above root 
    directories:

    mb_src:     - Maya scene files location (proj:work/gfxlib)
    tex_src:    - source textures location (proj:work/textures)

    model_dst:      - exported scene node files location (export:models)
    mesh_dst:       - exported mesh files location (export:meshes)
    anim_dst:       - exported anim files location (export:anims)
    tex_dst:        - exported texture files location (export:textures)
    physics_dst:    - exported physics and collide files (export:physics)

    Use Nebula3's IO::AssignRegistry and IO::IoServer objects to work
    with those assigns.

    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaDirectoryRegistry : public Core::RefCounted
{
    __DeclareClass(MayaDirectoryRegistry);
    __DeclareSingleton(MayaDirectoryRegistry);
public:
    /// constructor
    MayaDirectoryRegistry();
    /// destructor
    virtual ~MayaDirectoryRegistry();

    /// setup the directory registry
    void Setup();
    /// discard the directory registry
    void Discard();
    /// return true if the object is valid
    bool IsValid() const;

    /// update assigns to reflect external changes to the Windows registry
    void UpdateAssigns();
    /// set the project path to a new location(modifies Windows registry)
    void SetProjectPath(const Util::String& newProjectPath);
    /// get the current project path
    Util::String GetProjectPath() const;
    /// set the export path to a new location(modifies Windows registry)
    void SetExportPath(const Util::String& newExportPath);
    /// get the current export path
    Util::String GetExportPath() const;
	/// sets the current toolkit path
	void SetToolkitPath(const Util::String& newToolkitPath);
    /// get the current project path
    Util::String GetToolkitPath() const;
    /// get the temp path
    Util::String GetTempPath() const;

private:
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaDirectoryRegistry::IsValid() const
{
    return this->isValid;
}

} // namespace Maya
//------------------------------------------------------------------------------
    