#pragma once
//------------------------------------------------------------------------------
/**
    @class DataType
    
    Used to convert from and to strings and data type.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include <string>
#include "header.h"
namespace AnyFX
{

class DataType
{
public:

	enum Type
	{
		Float,
		Float2,
		Float3,
		Float4,
		Double,
		Double2,
		Double3,
		Double4,
		Integer,
		Integer2,
		Integer3,
		Integer4,
		UInteger,
		UInteger2,
		UInteger3,
		UInteger4,
		Short,
		Short2,
		Short3,
		Short4,
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
		Sampler1D,
		Sampler1DArray,
		Sampler2D,
		Sampler2DArray,
		Sampler2DMS,
		Sampler2DMSArray,
		Sampler3D,
		SamplerCube,
		SamplerCubeArray,
		Image1D,
		Image1DArray,
		Image2D,
		Image2DArray,
		Image2DMS,
		Image2DMSArray,
		Image3D,
		ImageCube,
		ImageCubeArray,
        AtomicCounter,
		Void,
		String,
		UserType,

		Undefined,

		NumTypes
	};

	enum Style
	{
		GLSL,
		HLSL,
		Generic,

		NumStyles
	};

	/// constructor
	DataType();
	/// destructor
	virtual ~DataType();

	/// equality operator
	bool operator==(const DataType& type) const;
	/// equality operator for convenience
	bool operator==(const Type& type) const;
	/// inequality operator
	bool operator!=(const DataType& type) const;
	/// inequality operator for convenience
	bool operator!=(const Type& type) const;

	/// sets type
	void SetType(const Type& type);
	/// gets type
	const Type& GetType() const;
	/// sets the variable style
	void SetStyle(const Style& style);
	/// gets the variable style
	const Style& GetStyle() const;
	/// sets name of defined type
	void SetName(const std::string& name);
	/// gets name of defined type
	const std::string& GetName() const;

    /// returns byte size for specific type
    static unsigned ToByteSize(const DataType& type);
	/// returns the primitive type of a data type
	static DataType ToPrimitiveType(const DataType& type);

	/// formats type to signature
	static std::string ToSignature(const DataType& type);
	/// formats type as string
	static std::string ToString(const DataType& type);
	/// translates type into vector size
	static unsigned ToVectorSize(const DataType& type);
	
	/// converts from a profile to a string
	static std::string ToProfileType(const DataType& dtype, const Header::Type& ttype);
private:

	/// converts type to string for glsl
	static std::string ToGLSLType(const DataType& type);
	/// converts type to string for hlsl
	static std::string ToHLSLType(const DataType& type);

	Style style;
	Type type;
	std::string name;

}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
DataType::SetType( const Type& type )
{
	this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType::Type& 
DataType::GetType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
DataType::SetStyle( const Style& style )
{
	this->style = style;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType::Style& 
DataType::GetStyle() const
{
	return this->style;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
DataType::SetName( const std::string& name )
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
DataType::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
DataType::operator==( const DataType& type ) const
{
	if (this->type == UserType && type.type == UserType)
	{
		return this->name == type.name;
	}
	else
	{
		return this->type == type.type;
	}	
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
DataType::operator==( const Type& type ) const
{
	return this->type == type;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
DataType::operator!=( const DataType& type ) const
{
	return this->type != type.type;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
DataType::operator!=( const Type& type ) const
{
	return this->type != type;
}
} // namespace AnyFX

//------------------------------------------------------------------------------
