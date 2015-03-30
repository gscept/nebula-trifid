#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::Character
    
    Provides information about a character's animations.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "coreanimation/animresource.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterAnimationLibrary
{
public:
    /// constructor
    CharacterAnimationLibrary();
    /// destructor
    ~CharacterAnimationLibrary();

    /// setup the animation lib
    void Setup(const Ptr<CoreAnimation::AnimResource>& animResource);
    /// discard the animation lib
    void Discard();
    /// return true if object is valid
    bool IsValid() const;

    /// get pointer to the character's animation resource
    const Ptr<CoreAnimation::AnimResource>& GetAnimResource() const;

private:
    Ptr<CoreAnimation::AnimResource> animResource;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterAnimationLibrary::IsValid() const
{
    return this->animResource.isvalid();
}

} // namespace Characters
//------------------------------------------------------------------------------
    