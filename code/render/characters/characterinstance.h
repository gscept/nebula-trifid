#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterInstance
    
    Contains the per-instance data of a character.
        
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "characters/characterskeletoninstance.h"
#include "characters/characterskinset.h"
#include "characters/charactervariationset.h"
#include "characters/characteranimationcontroller.h"
#include "jobs/jobport.h"

//------------------------------------------------------------------------------
namespace Characters
{
class Character;
    
class CharacterInstance : public Core::RefCounted
{
    __DeclareClass(CharacterInstance);
public:
    /// constructor
    CharacterInstance();
    /// destructor
    virtual ~CharacterInstance();

    /// setup the character instance from a character
    void Setup(const Ptr<Character>& origCharacter, const Ptr<Models::ModelInstance>& modelInst);
    /// discard the character instance
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// updates the time at which point the character should be animating
    void UpdateTime(const Timing::Tick time);

    /// get the character instance's job port
    const Ptr<Jobs::JobPort>& GetJobPort() const;
    /// access to get the character's skeleton
    CharacterSkeletonInstance& Skeleton();
    /// access to character's skin set
    CharacterSkinSet& SkinSet();
    /// access to character's animation controller
    CharacterAnimationController& AnimController();
    /// access to the character's texture set
    //CharacterTextureSet& TextureSet();
    /// access to the character's variation
    void SetVariationSetName(const Util::StringAtom& variationSetName);
    /// render a debug visualization of the character
    void RenderDebug(const Math::matrix44& modelTransform);
    /// wait for the character to become valid after StartUpdateAsync()
    void WaitUpdateDone() const;
    /// check whether asynchronous character update has finished
    bool CheckUpdateDone() const;
    /// return whether this object is valid for rendering (updated in StartUpdate()) 
    bool IsValidForRendering() const;
    /// get character joint texture row index (for GPUTextureSkinning)
    IndexT GetJointTextureRowIndex() const;

private:
    friend class CharacterServer;

    /// prepare the next update, returns true if update would be redundant
    bool PrepareUpdate(IndexT frameIndex);
    /// update the character instance
    void StartUpdate();

    Ptr<Character> character;
    Ptr<Models::ModelInstance> modelInstance;
    CharacterSkeletonInstance skeletonInst;
    CharacterSkinSet skinSet;
    CharacterAnimationController animController;
    IndexT updateFrameIndex;
    IndexT jointTextureRowIndex;
    Ptr<Jobs::JobPort> jobPort;
    bool isValidForRendering;
    //CharacterTextureSet textureSet;
    CharacterVariationSet variationSet;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterInstance::IsValid() const
{
    return this->character.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterInstance::IsValidForRendering() const
{
    return this->isValidForRendering;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterSkeletonInstance&
CharacterInstance::Skeleton()
{
    return this->skeletonInst;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterSkinSet&
CharacterInstance::SkinSet()
{
    return this->skinSet;
}

//------------------------------------------------------------------------------
/**
*/
inline CharacterAnimationController&
CharacterInstance::AnimController()
{
    return this->animController;
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT 
CharacterInstance::GetJointTextureRowIndex() const
{
    return this->jointTextureRowIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Jobs::JobPort>&
CharacterInstance::GetJobPort() const
{
    return this->jobPort;
}
} // namespace Characters
//------------------------------------------------------------------------------

    