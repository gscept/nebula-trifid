#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaSceneExtractor
    
    This is the top-level extractor which calls all other extractors.
    
    (C) 2009 Radon Labs GmbH
*/
#include "mayaextractors/mayaextractorbase.h"
#include "mayacore/mayascene.h"
#include "mayaextractors/mayahierarchyextractor.h"
#include "mayaextractors/mayalodnodeextractor.h"
#include "mayaextractors/mayashapenodeextractor.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaSceneExtractor : public MayaExtractorBase
{
    __DeclareClass(MayaSceneExtractor);
public:
    /// setup the object
    virtual void Setup();
    /// discard the objects
    virtual void Discard();

    /// called to extract scene data
    virtual MayaStatus::Code Extract(const Ptr<MayaScene>& mayaScene);

private:
    Ptr<MayaHierarchyExtractor> hierarchyExtractor;
    Ptr<MayaLodNodeExtractor> lodNodeExtractor;
    Ptr<MayaShapeNodeExtractor> shapeNodeExtractor;
};

} // namespace Maya
//------------------------------------------------------------------------------
    