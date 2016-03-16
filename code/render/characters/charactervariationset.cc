//------------------------------------------------------------------------------
//  CharacterVariationSet.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/charactervariationset.h"
#include "characters/charactervariationlibrary.h"

namespace Characters
{
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
CharacterVariationSet::CharacterVariationSet() :
    variationLibrary(0)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterVariationSet::~CharacterVariationSet()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterVariationSet::Setup(const Util::StringAtom& defaultVariationName, const CharacterVariationLibrary& varLib)
{
    n_assert(!this->IsValid());
    this->variationLibrary = &varLib;    
    this->variationName = defaultVariationName;
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterVariationSet::Discard()
{
    n_assert(this->IsValid());
    this->variationLibrary = 0;
}

//------------------------------------------------------------------------------
/**
*/
const Util::FixedArray<CharJointComponents>& 
CharacterVariationSet::GetVariationCharJointComponents() const
{                             
    n_assert(this->IsValid());
    return this->variationLibrary->GetVariationComponentsByName(this->variationName);
}

//------------------------------------------------------------------------------
/**
*/
void 
CharacterVariationSet::SetActiveVariation(const Util::StringAtom& variationName)
{
    n_assert(this->variationLibrary->HasVariation(variationName));
    this->variationName = variationName;
}

//------------------------------------------------------------------------------
/**
*/
const Util::StringAtom& 
CharacterVariationSet::GetActiveVariation() const
{
    return this->variationName;
}
} // namespace Characters
