#pragma once

#include "math/float2.h"
#include "math/line.h"
#include "math/vector.h"
#include "math/matrix44.h"

#include <Common/Base/hkBase.h>

//TODO: finish, only method names have been changed

//------------------------------------------------------------------------------
/**
*/
inline
Math::float4 Hk2NebFloat4(const hkVector4& hkVec)
{
	return Math::float4(hkVec(0), hkVec(1), hkVec(2), hkVec(3));
}

//------------------------------------------------------------------------------
/**
*/
inline
hkVector4 Neb2HkFloat4(const Math::float4& p)
{
	return hkVector4(p.x(), p.y(), p.z(), p.w());
}

//------------------------------------------------------------------------------
/**
*/
inline
Math::quaternion Hk2NebQuaternion(const hkQuaternion& hkQuat)
{
	return Math::quaternion(hkQuat.m_vec(0), hkQuat.m_vec(1), hkQuat.m_vec(2), hkQuat.m_vec(3));
}

//------------------------------------------------------------------------------
/**
*/
inline
hkMatrix4 Neb2HkMatrix44(const Math::matrix44& m)
{
	hkMatrix4 blah;
	n_error("BLAH2");
	return blah;

	//hkMatrix4 matrix;

	//matrix.setRows(	Neb2HkVector(m.getrow0()),
	//				Neb2HkVector(m.getrow1()),
	//				Neb2HkVector(m.getrow2()),
	//				Neb2HkVector(m.getrow3()));

	//return matrix;
}

//------------------------------------------------------------------------------
/**
*/
inline
Math::matrix44 Hk2NebMatrix44(const hkMatrix4& hkMat)
{
	Math::matrix44 blah;
	n_error("blah");
	return blah;

	//return Math::matrix44(	Math::float4(hkMat(0, 0), hkMat(0, 1), hkMat(0, 2), hkMat(0, 3)),
	//						Math::float4(hkMat(1, 0), hkMat(1, 1), hkMat(1, 2), hkMat(1, 3)),
	//						Math::float4(hkMat(2, 0), hkMat(2, 1), hkMat(2, 2), hkMat(2, 3)),
	//						Math::float4(hkMat(3, 0), hkMat(3, 1), hkMat(3, 2), hkMat(3, 3)));
}

//------------------------------------------------------------------------------
/**
*/
inline
hkQuaternion Neb2HkQuaternion(const Math::quaternion& q)
{
	return hkQuaternion(q.x(), q.y(), q.z(), q.w());
}

//------------------------------------------------------------------------------
/**
	Convert a Nebula matrix44 to a hkTransform, which consists of a hkVector4
	translation and a hkRotation (= hkMatrix3) rotation
*/
inline
hkTransform NebMatrix442HkTransform(const Math::matrix44& m)
{
	Math::quaternion rotation = Math::quaternion::normalize(Math::quaternion::rotationmatrix(m));
	const Math::float4& translation = m.get_position();

	return hkTransform(Neb2HkQuaternion(rotation), Neb2HkFloat4(translation));
}

//------------------------------------------------------------------------------
/**
	Convert a Nebula matrix44 to a hkQTransform, which consists of a hkVector4
	translation and a hkQuaternion rotation
*/
inline
hkQTransform NebMatrix442HkQTransform(const Math::matrix44& m)
{
	Math::quaternion rotation = Math::quaternion::normalize(Math::quaternion::rotationmatrix(m));
	const Math::float4& translation = m.get_position();

	return hkQTransform(Neb2HkQuaternion(rotation), Neb2HkFloat4(translation));
}

//------------------------------------------------------------------------------
/**
	Convert a Nebula matrix44 to a hkQsTransform, which consists of a hkVector4
	translation, a hkQuaternion rotation, and a hkVector4 scale
*/
inline
hkQsTransform NebMatrix442HkQsTransform(const Math::matrix44& m)
{
	Math::quaternion rotation = Math::quaternion::normalize(Math::quaternion::rotationmatrix(m));
	const Math::float4& translation = m.get_position();

	return hkQsTransform(Neb2HkFloat4(translation), Neb2HkQuaternion(rotation), hkVector4(1, 1, 1, 1));
}

//------------------------------------------------------------------------------
/**
	See NebMatrix442HkTransform comment
*/
inline
Math::matrix44 HkTransform2NebMatrix44(const hkTransform& t)
{
	hkQuaternion rotation = hkQuaternion(t.getRotation());	//< convert the hkRotation to a hkQuaternion
	Math::matrix44 nebMatrix = Math::matrix44::rotationquaternion(Hk2NebQuaternion(rotation));
	hkVector4 translation = t.getTranslation();
	Math::float4 nebTranslation = Hk2NebFloat4(translation);
	nebTranslation.set_w(1);
	nebMatrix.setrow3(nebTranslation);

	return nebMatrix;
}

//------------------------------------------------------------------------------
/**
	See NebMatrix442HkQTransform comment
*/
inline
Math::matrix44 HkQTransform2NebMatrix44(const hkQTransform& t)
{
	Math::matrix44 nebMatrix = Math::matrix44::rotationquaternion(Hk2NebQuaternion(t.getRotation()));
	hkVector4 translation = t.getTranslation();;
	Math::float4 nebTranslation = Hk2NebFloat4(translation);
	nebTranslation.set_w(1);
	nebMatrix.setrow3(nebTranslation);

	return nebMatrix;	
}

//------------------------------------------------------------------------------
/**
	See NebMatrix442HkQsTransform comment
*/
inline
Math::matrix44 HkQsTransform2NebMatrix44(const hkQsTransform& t)
{
	Math::vector scale = Hk2NebFloat4(t.getScale());
	Math::quaternion rotation = Hk2NebQuaternion(t.getRotation());
	Math::vector position = Hk2NebFloat4(t.getTranslation());

	Math::matrix44 mat = Math::matrix44::transformation(Math::float4::zerovector(), Math::quaternion::identity(), scale, Math::float4::zerovector(), rotation, position);
	return mat;
}

////------------------------------------------------------------------------------
///**
//*/
//inline
//Math::point Hk2NebPoint(const btVector3 &btV)
//{
//	return Math::point(btV.x(), btV.y(), btV.z());
//}
//
////------------------------------------------------------------------------------
///**
//*/
//inline
//Math::matrix44 Hk2NebMatrix3x3(const btMatrix3x3 &m)
//{
//	return Math::matrix44(Math::float4(m.getRow(0).x(), m.getRow(1).x(), m.getRow(2).x(), 0.0f), 
//		Math::float4(m.getRow(0).y(), m.getRow(1).y(), m.getRow(2).y(), 0.0f), 
//		Math::float4(m.getRow(0).z(), m.getRow(1).z(), m.getRow(2).z(), 0.0f), 
//		Math::float4(0.0f, 0.0f, 0.0f, 1.0f));
//}
//
////------------------------------------------------------------------------------
///**
//*/
//inline
//Math::matrix44 Hk2NebTransform(const btTransform &t)
//{
//	const btMatrix3x3 &rot = t.getBasis();
//	Math::matrix44 m = Hk2NebMatrix3x3(rot);
//	m.set_position(Hk2NebPoint(t.getOrigin()));
//	return m;
//}
//
////------------------------------------------------------------------------------
///**
//*/
//inline
//btMatrix3x3 Neb2HkM44M33(const Math::matrix44 &m)
//{
//	return btMatrix3x3(m.getrow0().x(), m.getrow1().x(), m.getrow2().x(), 
//		m.getrow0().y(), m.getrow1().y(), m.getrow2().y(), 
//		m.getrow0().z(), m.getrow1().z(), m.getrow2().z());
//}
//
//
//
////------------------------------------------------------------------------------
///**
//*/
//inline
//btVector3 Neb2HkPoint(const Math::point &p)
//{
//	return btVector3(p.x(), p.y(), p.z());
//}

////------------------------------------------------------------------------------
///**
//*/
//inline
//btTransform Neb2HkM44Transform(const Math::matrix44 &m)
//{
//	btTransform transform;
//	transform.setBasis(Neb2HkM44M33(m));
//	transform.setOrigin(Neb2HkPoint(m.get_position()));
//	return transform;
//}
