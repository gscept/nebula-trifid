#pragma once
//------------------------------------------------------------------------------
/**
    @class Shared::CharJointInfo
  
    Contains information about a character joint.
    
    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "util/stringatom.h"
#include "math/matrix44.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharJointInfo
{
public:
    /// default constructor
    CharJointInfo();
    /// constructor with name and joint index
    CharJointInfo(const Util::StringAtom& jointName, const Math::matrix44& localMatrix, const Math::matrix44& globalMatrix);
    
    /// set local matrix
    void SetLocalMatrix(const Math::matrix44& m);
    /// get joint matrix in global space
    const Math::matrix44& GetLocalMatrix() const;
    /// set global matrix
    void SetGlobalMatrix(const Math::matrix44& m);
    /// get joint matrix in global space
    const Math::matrix44& GetGlobalMatrix() const;
    /// set joint name
    void SetJointName(const Util::StringAtom& n);
    /// get joint name
    const Util::StringAtom& GetJointName() const;

private:
    Math::matrix44 localMatrix;
    Math::matrix44 globalMatrix;
    Util::StringAtom jointName;
};

//------------------------------------------------------------------------------
/**
*/
inline
CharJointInfo::CharJointInfo() :
    localMatrix(Math::matrix44::identity()),
    globalMatrix(Math::matrix44::identity())
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
CharJointInfo::CharJointInfo(const Util::StringAtom& n, const Math::matrix44& l, const Math::matrix44& g) :
    localMatrix(l),
    globalMatrix(g),
    jointName(n)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharJointInfo::SetJointName(const Util::StringAtom& n)
{
    this->jointName = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
CharJointInfo::GetJointName() const
{
    return this->jointName;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharJointInfo::SetLocalMatrix(const Math::matrix44& m)
{
    this->localMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CharJointInfo::GetLocalMatrix() const
{
    return this->localMatrix;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CharJointInfo::SetGlobalMatrix(const Math::matrix44& m)
{
    this->globalMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
CharJointInfo::GetGlobalMatrix() const
{
    return this->globalMatrix;
}

} // namespace Shared
//------------------------------------------------------------------------------
