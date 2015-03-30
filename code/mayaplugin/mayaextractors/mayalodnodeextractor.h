#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaLodNodeExtractor
  
    Extracts LOD node information from the Maya scene. Depends on the
    information already extracted by the MayaHierarchyExtractor.
    
    (C) 2009 Radon Labs GmbH
*/    
#include "mayaextractors/mayaextractorbase.h"
#include <maya/MDagPath.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaLodNodeExtractor : public MayaExtractorBase
{
    __DeclareClass(MayaLodNodeExtractor);
public:
    /// extract hierarchy data from the scene
    virtual MayaStatus::Code Extract(const Ptr<MayaScene>& mayaScene);
private:
    /// extract a single node and recurse
    void ExtractNode(const Ptr<MayaScene>& mayaScene, const MDagPath& nodePath);
};

} // namespace Maya
//------------------------------------------------------------------------------
