#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaExporterBase
  
    Base for Maya exporter classes.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include "util/string.h"
#include "mayacore/mayastatus.h"
#include <maya/MDagPath.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaExporterBase
{
public:
    /// constructor
    MayaExporterBase();
    /// destructor
    virtual ~MayaExporterBase();

    /// set target directory
    void SetTargetDirectory(const Util::String& dir);
    /// get target directory
    const Util::String& GetTargetDirectory() const;
    /// set target file name
    void SetTargetFile(const Util::String& dir);
    /// get target file name
    const Util::String& GetTargetFile() const;
    /// set root node path
    void SetRootNode(const Util::String& node);
    /// get root node path
    const Util::String& GetRootNode() const;
    /// check if root node exists in the scene
    bool IsRootNodeValid() const;
    /// get the dag path to the root node
    const MDagPath& GetRootNodeDagPath() const;

    /// perform exporting
    virtual MayaStatus::Code Export();
    /// save result to filesystem
    virtual bool Save();

private:
    /// update the root node dag path from root node name
    void UpdateRootNodeDagPath();
        
    Util::String dstDir;
    Util::String dstFile;
    Util::String rootNode;
    MDagPath rootNodeDagPath;
};

//------------------------------------------------------------------------------
/**
*/
inline void
MayaExporterBase::SetTargetDirectory(const Util::String& str)
{
    this->dstDir = str;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaExporterBase::GetTargetDirectory() const
{
    return this->dstDir;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MayaExporterBase::SetTargetFile(const Util::String& str)
{
    this->dstFile = str;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaExporterBase::GetTargetFile() const
{
    return this->dstFile;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MayaExporterBase::SetRootNode(const Util::String& str)
{
    this->rootNode = str;
    this->UpdateRootNodeDagPath();
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::String&
MayaExporterBase::GetRootNode() const
{
    return this->rootNode;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaExporterBase::IsRootNodeValid() const
{
    return this->rootNodeDagPath.isValid();
}

//------------------------------------------------------------------------------
/**
*/
inline const MDagPath&
MayaExporterBase::GetRootNodeDagPath() const
{
    return this->rootNodeDagPath;
}

} // namespace Maya
//------------------------------------------------------------------------------
