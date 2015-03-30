#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaNodeUtil
    
    Utility methods for Maya scene nodes.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaNodeUtil
{
public:
    /// test if node is valid for export (mainly checks visibility)
    static bool IsExportNode(const MDagPath& dagNode);
    /// test if node is a hierarchy node
    static bool IsHierarchyNode(const MDagPath& dagNode);
    /// lookup a node by name, may contain path and wildcards
    static MDagPathArray LookupNode(const MString& path);
    /// find the closest parent hierarchy node of a dag path
    static bool LookupParentHierarchyNode(const MDagPath& dagNode, MDagPath& outDagPath);
    /// get the name of a node
    static MString GetNodeName(const MObject& node);
};

} // namespace Maya
//------------------------------------------------------------------------------

    