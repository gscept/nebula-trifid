//------------------------------------------------------------------------------
//  mayascenextractor.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaextractors/mayasceneextractor.h"

namespace Maya
{
__ImplementClass(Maya::MayaSceneExtractor, 'MSCX', Maya::MayaExtractorBase);

//------------------------------------------------------------------------------
/**
*/
void
MayaSceneExtractor::Setup()
{
    n_assert(!this->IsValid());
    MayaExtractorBase::Setup();  

    this->hierarchyExtractor = MayaHierarchyExtractor::Create();
    this->hierarchyExtractor->Setup();
    
    this->lodNodeExtractor = MayaLodNodeExtractor::Create();
    this->lodNodeExtractor->Setup();

    this->shapeNodeExtractor = MayaShapeNodeExtractor::Create();
    this->shapeNodeExtractor->Setup();
}

//------------------------------------------------------------------------------
/**
*/
void
MayaSceneExtractor::Discard()
{
    n_assert(this->IsValid());

    this->shapeNodeExtractor->Discard();
    this->shapeNodeExtractor = 0;

    this->lodNodeExtractor->Discard();
    this->lodNodeExtractor = 0;

    this->hierarchyExtractor->Discard();
    this->hierarchyExtractor = 0;
    
    MayaExtractorBase::Discard();
}

//------------------------------------------------------------------------------
/**
*/
MayaStatus::Code
MayaSceneExtractor::Extract(const Ptr<MayaScene>& mayaScene)
{
    n_assert(this->IsValid());
    if (MayaStatus::Error == this->hierarchyExtractor->Extract(mayaScene))
    {
        return MayaStatus::Error;
    }
    if (MayaStatus::Error == this->lodNodeExtractor->Extract(mayaScene))
    {
        return MayaStatus::Error;
    }
    if (MayaStatus::Error == this->shapeNodeExtractor->Extract(mayaScene))
    {
        return MayaStatus::Error;
    }
    return MayaStatus::Success;
}

} // namespace Maya