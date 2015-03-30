//------------------------------------------------------------------------------
//  mayaextractorbase.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayaextractors/mayaextractorbase.h"

namespace Maya
{
__ImplementClass(Maya::MayaExtractorBase, 'MEXB', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
MayaExtractorBase::MayaExtractorBase() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
MayaExtractorBase::~MayaExtractorBase()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
MayaExtractorBase::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaExtractorBase::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
*/
MayaStatus::Code
MayaExtractorBase::Extract(const Ptr<MayaScene>& mayaScene)
{
    return MayaStatus::NothingTodo;
}

} // namespace Maya