//------------------------------------------------------------------------------
//  characteranimationlibrary.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/characteranimationlibrary.h"

namespace Characters
{
using namespace CoreAnimation;

//------------------------------------------------------------------------------
/**
*/
CharacterAnimationLibrary::CharacterAnimationLibrary()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharacterAnimationLibrary::~CharacterAnimationLibrary()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationLibrary::Setup(const Ptr<AnimResource>& animRes)
{
    n_assert(!this->IsValid());
    n_assert(animRes.isvalid());
    this->animResource = animRes;
}

//------------------------------------------------------------------------------
/**
*/
void
CharacterAnimationLibrary::Discard()
{
    n_assert(this->IsValid());
    this->animResource = 0;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<AnimResource>&
CharacterAnimationLibrary::GetAnimResource() const
{
    return this->animResource;
}

} // namespace Characters
