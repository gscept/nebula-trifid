//------------------------------------------------------------------------------
//  character.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/character.h"

namespace Characters
{
__ImplementClass(Characters::Character, 'CRTR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
Character::Character() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Character::~Character()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
    NOTE: embedded objects must still be setup individually after this
    method has been called!
*/
void
Character::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Character::Discard()
{
    n_assert(this->IsValid());

    // discard embedded objects
    if (this->skeleton.IsValid())
    {
        this->skeleton.Discard();
    }
    if (this->animationLibrary.IsValid())
    {
        this->animationLibrary.Discard();
    }
    if (this->variationLibrary.IsValid())
    {
        this->variationLibrary.Discard();
    }
    this->skinLibrary.Discard();
    this->isValid = false;
}

} // namespace Characters
