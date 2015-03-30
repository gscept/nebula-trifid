#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VarType
    
    Encapsulates a variable value type, such as float, int, bool etc.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/types.h"
#include "util/string.h"
namespace Nody
{
class VarType
{
public:

    // do not change the intrinsic order of the types, we want floats, int, bool, matrices in this order
	enum ValueType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Bool2,
		Bool3,
		Bool4,
		Matrix2x2,
		Matrix2x3,
		Matrix2x4,
		Matrix3x2,
		Matrix3x3,
		Matrix3x4,
		Matrix4x2,
		Matrix4x3,
		Matrix4x4,
        Sampler2D,
        Sampler2DArray,
        Sampler3D,
        Sampler3DArray,
        SamplerCube,
        SamplerCubeArray,
        Any,                // fuzzy type, can basically be anything
        AnyVec,             // fuzzy vector type, can be any vector form
        AnyMat,             // fuzzy matrix type, can be any type of matrix

		UnknownType,

		NumValueTypes
	};

	/// constructor
	VarType();
	/// copy constructor
	VarType(const VarType& rhs);
	/// destructor
	virtual ~VarType();
	
	/// equality operator
	bool operator==(const VarType& rhs) const;
	/// inequality operator
	bool operator!=(const VarType& rhs) const;

	/// tests if variables can be intrinsically mapped
	bool CanMap(const VarType& rhs) const;

    /// returns if type is a vector type
    static bool IsVector(const VarType& type);
    /// returns vector size
    static uint VectorSize(const VarType& type);
    /// returns the unvectorized type
    static ValueType ComponentType(const VarType& type);
	
	/// get value type
	const ValueType& GetType() const;
    /// compare type
    const bool IsA(const ValueType& type) const;
	/// get byte size
	const uint GetSize() const;

	/// parse and return value type from string
	static VarType FromString(const Util::String& string);
	/// convert vartype to string
	static Util::String ToString(const VarType& type);

private:
	uint byteSize;
	ValueType type;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const VarType::ValueType& 
VarType::GetType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint 
VarType::GetSize() const
{
	return this->byteSize;
}

} // namespace Nody
//------------------------------------------------------------------------------