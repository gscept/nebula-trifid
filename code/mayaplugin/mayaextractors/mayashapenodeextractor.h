#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaShapeNodeExtractor
    
    Scene extractor for polygon shape nodes.
    
    (C) 2009 Radon Labs GmbH
*/
#include "mayaextractors/mayaextractorbase.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaShapeNodeExtractor : public MayaExtractorBase
{
    __DeclareClass(MayaShapeNodeExtractor);
public:
    /// extract hierarchy data from the scene
    virtual MayaStatus::Code Extract(const Ptr<MayaScene>& mayaScene);
};

} // namespace Maya
//------------------------------------------------------------------------------
