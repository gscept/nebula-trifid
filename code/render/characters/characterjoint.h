#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterJoint
    
    Holds shared data of a character joint.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/stringatom.h"
#include "math/point.h"
#include "math/vector.h"
#include "math/quaternion.h"
#include "math/matrix44.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterJoint
{
public:
    /// constructor
    CharacterJoint();
    /// destructor
    ~CharacterJoint();
    /// setup the joint
    void Setup(IndexT parentJointIndex, const CharacterJoint* parentJoint, const Math::vector& poseTranslation, const Math::quaternion& poseRotation, const Math::vector& poseScale, const Util::StringAtom& name, Math::matrix44* invPoseMatrixPtr);
    
    /// get the joint's name
    const Util::StringAtom& GetName() const;
    /// return true if the joint has a parent joint
    bool HasParentJoint() const;
    /// get the parent joint index
    IndexT GetParentJointIndex() const;
    /// get pointer to parent joint (can be 0!)
    const CharacterJoint* GetParentJoint() const;
    /// get pose translation
    const Math::vector& GetPoseTranslation() const;
    /// get pose rotation
    const Math::quaternion& GetPoseRotation() const;
    /// get pose scale
    const Math::vector& GetPoseScale() const;
    /// get the pose matrix
    const Math::matrix44& GetPoseMatrix() const;
    /// get the inverse pose matrix
    const Math::matrix44& GetInvPoseMatrix() const;

private:
    Math::vector poseTranslation;
    Math::quaternion poseRotation;
    Math::vector poseScale;
    Math::matrix44 poseMatrix;
    Math::matrix44* invPoseMatrixPtr;
    IndexT parentJointIndex;
    const CharacterJoint* parentJoint;
    Util::StringAtom name;
};

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
CharacterJoint::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
CharacterJoint::HasParentJoint() const
{
    return (InvalidIndex != this->parentJointIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline IndexT
CharacterJoint::GetParentJointIndex() const
{
    return this->parentJointIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline const CharacterJoint*
CharacterJoint::GetParentJoint() const
{
    return this->parentJoint;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
CharacterJoint::GetPoseTranslation() const
{
    return this->poseTranslation;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::quaternion&
CharacterJoint::GetPoseRotation() const
{
    return this->poseRotation;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
CharacterJoint::GetPoseScale() const
{
    return this->poseScale;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CharacterJoint::GetPoseMatrix() const
{
    return this->poseMatrix;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CharacterJoint::GetInvPoseMatrix() const
{
    n_assert(0 != this->invPoseMatrixPtr);
    return *this->invPoseMatrixPtr;
}

} // namespace Characters
//------------------------------------------------------------------------------
    