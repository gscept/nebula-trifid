#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::Character

    A Character resource object holds all shared data belonging to a 
    skinned character. 

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "characters/characterskeleton.h"
#include "characters/characterskinlibrary.h"
#include "characters/characteranimationlibrary.h"
#include "characters/charactervariationlibrary.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterVariationLibrary;
class CharacterTextureLibrary;

class Character : public Core::RefCounted
{
    __DeclareClass(Character);
public:
    /// constructor
    Character();
    /// destructor
    virtual ~Character();

    /// setup the character object
    void Setup();
    /// discard the character object
    void Discard();
    /// return true if character object has been setup
    bool IsValid() const;

    /// access to the character's skeleton
    CharacterSkeleton& Skeleton();
    /// access to the character's skin library
    CharacterSkinLibrary& SkinLibrary();
    /// access to the character's anim library
    CharacterAnimationLibrary& AnimationLibrary();
    /// access to the character's variation library
    CharacterVariationLibrary& VariationLibrary();
    /// access to the character's texture library
    //CharacterTextureLibrary& TextureLibrary();

private:
    CharacterSkeleton skeleton;
    CharacterSkinLibrary skinLibrary;
    CharacterAnimationLibrary animationLibrary;
    CharacterVariationLibrary variationLibrary;
    //CharacterTextureLibrary textureLibrary;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Character::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterSkeleton&
Character::Skeleton()
{
    return this->skeleton;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterSkinLibrary&
Character::SkinLibrary()
{
    return this->skinLibrary;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterAnimationLibrary&
Character::AnimationLibrary()
{
    return this->animationLibrary;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterVariationLibrary& 
Character::VariationLibrary()
{
    return this->variationLibrary;
}
} // namespace Characters
//------------------------------------------------------------------------------



    