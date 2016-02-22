#pragma once
//------------------------------------------------------------------------------
/**
	PhysX utils for conversion of datatypes

	(C) 2016 Individual contributors, see AUTHORS file
*/

#include "foundation/PxVec3.h"
#include "math/vector.h"
#include "foundation/PxVec4.h"
#include "foundation/PxMat44.h"

//------------------------------------------------------------------------------
/**
*/
inline physx::PxVec3
Neb2PxVec(const Math::vector& vec)
{
	return physx::PxVec3(vec.x(), vec.y(), vec.z());
}

//------------------------------------------------------------------------------
/**
*/
inline  Math::vector
Px2NebVec(const physx::PxVec3& vec)
{
	return Math::vector(vec.x, vec.y, vec.z);
}

//------------------------------------------------------------------------------
/**
*/
inline physx::PxVec4
Neb2PxVec4(const Math::float4& vec)
{
	return physx::PxVec4(vec.x(), vec.y(), vec.z(), vec.w());
}

//------------------------------------------------------------------------------
/**
*/
inline physx::PxQuat
Neb2PxQuat(const Math::quaternion& vec)
{
	return physx::PxQuat(vec.x(), vec.y(), vec.z(), vec.w());
}

//------------------------------------------------------------------------------
/**
*/
inline physx::PxMat44
Neb2PxMat(const Math::matrix44& mat)
{
	return physx::PxMat44(Neb2PxVec4(mat.row0()), Neb2PxVec4(mat.row1()), Neb2PxVec4(mat.row2()), Neb2PxVec4(mat.row3()));
}

//------------------------------------------------------------------------------
/**
*/
inline physx::PxTransform
Neb2PxTrans(const Math::matrix44& mat)
{
	return physx::PxTransform(Neb2PxMat(mat));
}