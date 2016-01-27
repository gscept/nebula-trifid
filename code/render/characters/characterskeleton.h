#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterSkeleton
    
    Contains the skeleton data of a character which is shared between all
    instances of the character.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "characters/characterjoint.h"
#include "util/fixedarray.h"
#include "util/dictionary.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterSkeleton
{
public:
    /// constructor
    CharacterSkeleton();
    /// destructor
    ~CharacterSkeleton();
    
    /// setup the skeleton
    void Setup(SizeT numJoints);
    /// setup a joint in the skeleton
    void SetupJoint(IndexT jointIndex, IndexT parentJointIndex, const Math::point& poseTranslation, const Math::quaternion& poseRotation, const Math::vector& poseScale, const Util::StringAtom& name);
    /// discard the skeleton
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;

    /// get number of joints in the skeleton
    SizeT GetNumJoints() const;
    /// access to a joint in the skeleton
    const CharacterJoint& GetJoint(IndexT index) const;
	/// access to the root joint
	const CharacterJoint& GetRootJoint() const;
    /// get a joint index by name, returns InvalidIndex, if joint not found
    IndexT GetJointIndexByName(const Util::StringAtom& jointName);
    /// get the invPoseMatrixArray (contains inverse joint pose matrices)
    const Util::FixedArray<Math::matrix44>& GetInvPoseMatrixArray() const;
    /// get pointer to default samples if no valid anim is set on character 
    const Util::FixedArray<Math::float4>& GetDefaultSamplesArray() const;

private:
    Util::FixedArray<Math::float4> defaultSamplesArray;  // used as sample result if no valid animation exists to provide samples
    Util::FixedArray<Math::matrix44> invPoseMatrixArray;
    Util::FixedArray<CharacterJoint> jointArray;
    Util::Dictionary<Util::StringAtom, IndexT> jointIndexMap;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterSkeleton::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
CharacterSkeleton::GetNumJoints() const
{
    return this->jointArray.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const CharacterJoint& 
CharacterSkeleton::GetJoint(IndexT index) const
{
    return this->jointArray[index];
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
CharacterSkeleton::GetJointIndexByName(const Util::StringAtom& jointName)
{
    IndexT i = this->jointIndexMap.FindIndex(jointName);
    if (InvalidIndex == i)
    {
        return InvalidIndex;
    }
    else
    {
        return this->jointIndexMap.ValueAtIndex(i);
    }
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::FixedArray<Math::matrix44>&
CharacterSkeleton::GetInvPoseMatrixArray() const
{
    return this->invPoseMatrixArray;
}

//------------------------------------------------------------------------------
/**
    Get pointer to the default samples array. This is used if no
    animation is set on the character as input to the skeleton
    evaluation. 
    Format is always translation, rotation, scale, velocity.
*/
inline const Util::FixedArray<Math::float4>&
CharacterSkeleton::GetDefaultSamplesArray() const
{
    return this->defaultSamplesArray;
}

} // namespace Characters
//------------------------------------------------------------------------------
    