#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaType
    
    Utility class for converting between Maya and Nebula3 data types. Careful,
    some conversion can be very expensive (for instance converting between
    large arrays!).
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/types.h"
#include "util/string.h"
#include "util/array.h"
#include "util/stringatom.h"
#include "math/vector.h"
#include "math/point.h"
#include "math/quaternion.h"
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MIntArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>

//------------------------------------------------------------------------------
namespace Maya
{
class MayaType
{
public:
    /// cast from one type to another
    template<class FROM_TYPE, class TO_TYPE> static TO_TYPE Cast(const FROM_TYPE& from);
};

//------------------------------------------------------------------------------
/**
    Convert from N3 string to Maya string.
*/
template<> inline MString
MayaType::Cast<Util::String, MString>(const Util::String& from)
{
    return MString(from.AsCharPtr());
}

//------------------------------------------------------------------------------
/**
    Convert from Maya string to N3 string.
*/
template<> inline Util::String
MayaType::Cast<MString, Util::String>(const MString& from)
{
    return Util::String(from.asChar());
}

//------------------------------------------------------------------------------
/**
    Convert from Maya string to N3 StringAtom.
*/
template<> inline Util::StringAtom
MayaType::Cast<MString, Util::StringAtom>(const MString& from)
{
    return Util::StringAtom(from.asChar());
}

//------------------------------------------------------------------------------
/**
    Convert from N3 StringAtom to Maya string.
*/
template<> inline MString
MayaType::Cast<Util::StringAtom, MString>(const Util::StringAtom& from)
{
    return MString(from.Value());
}

//------------------------------------------------------------------------------
/**
    Convert from MStringArray to Array<String>
*/
template<> inline Util::Array<Util::String>
MayaType::Cast<MStringArray, Util::Array<Util::String>>(const MStringArray& from)
{
    Util::Array<Util::String> result;
    SizeT size = from.length();
    result.Reserve(size);
    IndexT i;
    for (i = 0; i < size; i++)
    {
        result.Append(from[i].asChar());
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert from Array<String> to MStringArray.
*/
template<> inline MStringArray
MayaType::Cast<Util::Array<Util::String>, MStringArray>(const Util::Array<Util::String>& from)
{
    MStringArray result;
    SizeT size = from.Size();
    IndexT i;
    for (i = 0; i < size; i++)
    {
        result.append(from[i].AsCharPtr());
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert MIntArray to Array<int>.
*/
template<> inline Util::Array<int>
MayaType::Cast<MIntArray, Util::Array<int>>(const MIntArray& from)
{
    Util::Array<int> result;
    SizeT size = from.length();
    result.Reserve(size);
    IndexT i;
    for (i = 0; i < size; i++)
    {
        result.Append(from[i]);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert Array<int> to MIntArray.
*/
template<> inline MIntArray
MayaType::Cast<Util::Array<int>, MIntArray>(const Util::Array<int>& from)
{
    MIntArray result;
    SizeT size = from.Size();
    IndexT i;
    for (i = 0; i < size; i++)
    {
        result.append(from[i]);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert MDoubleArray to Array<float>.
*/
template<> inline Util::Array<float>
MayaType::Cast<MDoubleArray, Util::Array<float>>(const MDoubleArray& from)
{
    Util::Array<float> result;
    SizeT size = from.length();
    result.Reserve(size);
    IndexT i;
    for (i = 0; i < size; i++)
    {
        result.Append(float(from[i]));
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert Array<float> to MDoubleArray.
*/
template<> inline MDoubleArray
MayaType::Cast<Util::Array<float>, MDoubleArray>(const Util::Array<float>& from)
{
    MDoubleArray result;
    SizeT size = from.Size();
    IndexT i;
    for (i = 0; i < size; i++)
    {
        result.append(from[i]);
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert MVectorArray to Array<Math::vector>.
*/
template<> inline Util::Array<Math::vector>
MayaType::Cast<MVectorArray, Util::Array<Math::vector>>(const MVectorArray& from)
{
    Util::Array<Math::vector> result;
    SizeT size = from.length();
    result.Reserve(size);
    IndexT i;
    for (i = 0; i < size; i++)
    {
        const MVector& vec = from[i];
        result.Append(Math::vector(Math::scalar(vec.x), Math::scalar(vec.y), Math::scalar(vec.z)));
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert Array<Math::vector> to MVectorArray.
*/
template<> inline MVectorArray
MayaType::Cast<Util::Array<Math::vector>, MVectorArray>(const Util::Array<Math::vector>& from)
{
    MVectorArray result;
    SizeT size = from.Size();
    IndexT i;
    for (i = 0; i < size; i++)
    {
        const Math::vector vec = from[i];
        result.append(MVector(vec.x(), vec.y(), vec.z()));
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert MVectorArray to Array<Math::vector>.
*/
template<> inline Util::Array<Math::point>
MayaType::Cast<MPointArray, Util::Array<Math::point>>(const MPointArray& from)
{
    Util::Array<Math::point> result;
    SizeT size = from.length();
    result.Reserve(size);
    IndexT i;
    for (i = 0; i < size; i++)
    {
        const MPoint& pnt = from[i];
        result.Append(Math::point(Math::scalar(pnt.x), Math::scalar(pnt.y), Math::scalar(pnt.z)));
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert Array<Math::point> to MPointArray.
*/
template<> inline MPointArray
MayaType::Cast<Util::Array<Math::point>, MPointArray>(const Util::Array<Math::point>& from)
{
    MPointArray result;
    SizeT size = from.Size();
    IndexT i;
    for (i = 0; i < size; i++)
    {
        const Math::point& pnt = from[i];
        result.append(MPoint(pnt.x(), pnt.y(), pnt.z()));
    }
    return result;
}

//------------------------------------------------------------------------------
/**
    Convert Maya MMatrix to Nebula3 Math::matrix44.
*/
template<> inline Math::matrix44
MayaType::Cast<MMatrix, Math::matrix44>(const MMatrix& in)
{
    Math::matrix44 out;
    out.setrow0(Math::float4(float(in[0][0]), float(in[0][1]), float(in[0][2]), float(in[0][3])));
    out.setrow1(Math::float4(float(in[1][0]), float(in[1][1]), float(in[1][2]), float(in[1][3])));
    out.setrow2(Math::float4(float(in[2][0]), float(in[2][1]), float(in[2][2]), float(in[2][3])));
    out.setrow3(Math::float4(float(in[3][0]), float(in[3][1]), float(in[3][2]), float(in[3][3])));
    return out;
}

//------------------------------------------------------------------------------
/**
    Convert Nebula3 Math::matrix44 to Maya MMatrix.
*/
template<> inline MMatrix
MayaType::Cast<Math::matrix44, MMatrix>(const Math::matrix44& in)
{
    MMatrix out;
    const Math::float4& r0 = in.getrow0();
    const Math::float4& r1 = in.getrow1();
    const Math::float4& r2 = in.getrow2();
    const Math::float4& r3 = in.getrow3();
    out[0][0] = r0.x(); out[0][1] = r0.y(); out[0][2] = r0.z(); out[0][3] = r0.w();
    out[1][0] = r1.x(); out[1][1] = r1.y(); out[1][2] = r1.z(); out[1][3] = r1.w();
    out[2][0] = r2.x(); out[2][1] = r2.y(); out[2][2] = r2.z(); out[2][3] = r2.w();
    out[3][0] = r3.x(); out[3][1] = r3.y(); out[3][2] = r3.z(); out[3][3] = r3.w();
    return out;
}

//------------------------------------------------------------------------------
/**
    Convert an MVector into a Math::vector.
*/
template<> inline Math::vector
MayaType::Cast<MVector, Math::vector>(const MVector& in)
{
    return Math::vector(float(in.x), float(in.y), float(in.z));
}

//------------------------------------------------------------------------------
/**
    Convert a Math::vector into a MVector.
*/
template<> inline MVector
MayaType::Cast<Math::vector, MVector>(const Math::vector& in)
{
    return MVector(in.x(), in.y(), in.z());
}

//------------------------------------------------------------------------------
/**
    Convert an MPoint into a Math::point.
*/
template<> inline Math::point
MayaType::Cast<MPoint, Math::point>(const MPoint& in)
{
    return Math::point(float(in.x), float(in.y), float(in.z));
}

//------------------------------------------------------------------------------
/**
    Convert a Math::point into a MPoint.
*/
template<> inline MPoint
MayaType::Cast<Math::point, MPoint>(const Math::point& in)
{
    return MPoint(in.x(), in.y(), in.z());
}

//------------------------------------------------------------------------------
/**
    Convert a MQuaternion into a Math::quaternion.
*/
template<> inline Math::quaternion
MayaType::Cast<MQuaternion, Math::quaternion>(const MQuaternion& in)
{
    return Math::quaternion(Math::scalar(in.x), Math::scalar(in.y), Math::scalar(in.z), Math::scalar(in.w));
}

//------------------------------------------------------------------------------
/**
    Convert a Math::quaternion into a MQuaternion.
*/
template<> inline MQuaternion
MayaType::Cast<Math::quaternion, MQuaternion>(const Math::quaternion& in)
{
    return MQuaternion(in.x(), in.y(), in.z(), in.w());
}

//------------------------------------------------------------------------------
/**
    Convert a Math::quaternion into a Math::float4.
*/
template<> inline Math::float4
MayaType::Cast<Math::quaternion, Math::float4>(const Math::quaternion& in)
{
    return Math::float4(in.x(), in.y(), in.z(), in.w());
}

} // namespace Maya
//------------------------------------------------------------------------------
