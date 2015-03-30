#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaHierarchyExtractor
  
    This extracts the transform hierarchy from the Maya scene and adds
    it to the MayaScene singleton. A simple Maya scene will only consist 
    of a single transform node at the top. Only scenes with hierarchy
    nodes will create an actual transform hierarchy in the MayaScene object.
    
    (C) 2009 Radon Labs GmbH
*/
#include "mayaextractors/mayaextractorbase.h"
#include <maya/MDagPath.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaHierarchyExtractor : public MayaExtractorBase
{
    __DeclareClass(MayaHierarchyExtractor);
public:
    /// extract hierarchy data from the scene
    virtual MayaStatus::Code Extract(const Ptr<MayaScene>& mayaScene);
private:
    /// extract a single node and recurse
    void ExtractNode(const Ptr<MayaScene>& mayaScene, const MDagPath& nodePath);
};

} // namespace MayaHierarchyExtractor
//------------------------------------------------------------------------------

