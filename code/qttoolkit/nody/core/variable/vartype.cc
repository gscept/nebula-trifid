//------------------------------------------------------------------------------
//  vartype.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vartype.h"

namespace Nody
{

//------------------------------------------------------------------------------
/**
*/
VarType::VarType() :
    byteSize(0),
    type(UnknownType)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/

VarType::VarType( const VarType& rhs ) :
	byteSize(rhs.byteSize),
	type(rhs.type)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VarType::~VarType()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
VarType::operator==( const VarType& rhs ) const
{
	return this->type == rhs.type;
}

//------------------------------------------------------------------------------
/**
*/
bool 
VarType::operator!=( const VarType& rhs ) const
{
	return this->type != rhs.type;
}

//------------------------------------------------------------------------------
/**
*/
Nody::VarType 
VarType::FromString( const Util::String& string )
{
	VarType retval;
	if (string == "float")
	{
		retval.type = Float;
		retval.byteSize = sizeof(float);
	}
	else if (string == "vec2")
	{
		retval.type = Float2;
		retval.byteSize = sizeof(float) * 2;
	}
	else if (string == "vec3")
	{
		retval.type = Float3;
		retval.byteSize = sizeof(float) * 3;
	}
	else if (string == "vec4")
	{
		retval.type = Float4;
		retval.byteSize = sizeof(float) * 4;
	}
	else if (string == "int")
	{
		retval.type = Int;
		retval.byteSize = sizeof(int);
	}
	else if (string == "ivec2")
	{
		retval.type = Int2;
		retval.byteSize = sizeof(int) * 2;
	}
	else if (string == "ivec3")
	{
		retval.type = Int3;
		retval.byteSize = sizeof(int) * 3;
	}
	else if (string == "ivec4")
	{
		retval.type = Int4;
		retval.byteSize = sizeof(int) * 4;
	}
	else if (string == "bool")
	{
		retval.type = Bool;
		retval.byteSize = sizeof(bool);
	}
	else if (string == "bvec2")
	{
		retval.type = Bool2;
		retval.byteSize = sizeof(bool) * 2;
	}
	else if (string == "bvec3")
	{
		retval.type = Bool3;
		retval.byteSize = sizeof(bool) * 3;
	}
	else if (string == "bvec4")
	{
		retval.type = Bool4;
		retval.byteSize = sizeof(bool) * 4;
	}
	else if (string == "mat2x2")
	{
		retval.type = Matrix2x2;
		retval.byteSize = sizeof(float) * 4;
	}
	else if (string == "mat2x3")
	{
		retval.type = Matrix2x3;
		retval.byteSize = sizeof(float) * 6;
	}
	else if (string == "mat2x4")
	{
		retval.type = Matrix2x4;
		retval.byteSize = sizeof(float) * 8;
	}
	else if (string == "mat3x2")
	{
		retval.type = Matrix3x2;
		retval.byteSize = sizeof(float) * 6;
	}
	else if (string == "mat3x3")
	{
		retval.type = Matrix3x3;
		retval.byteSize = sizeof(float) * 9;
	}
	else if (string == "mat3x4")
	{
		retval.type = Matrix3x4;
		retval.byteSize = sizeof(float) * 12;
	}
	else if (string == "mat4x2")
	{
		retval.type = Matrix4x2;
		retval.byteSize = sizeof(float) * 8;
	}
	else if (string == "mat4x3")
	{
		retval.type = Matrix4x3;
		retval.byteSize = sizeof(float) * 12;
	}
	else if (string == "mat4x4")
	{
		retval.type = Matrix4x4;
		retval.byteSize = sizeof(float) * 16;
	}
    else if (string == "sampler2D")
    {
        retval.type = Sampler2D;
        retval.byteSize = -1;
    }
    else if (string == "sampler2DArray")
    {
        retval.type = Sampler2DArray;
        retval.byteSize = -1;
    }
    else if (string == "sampler3D")
    {
        retval.type = Sampler3D;
        retval.byteSize = -1;
    }
    else if (string == "sampler3DArray")
    {
        retval.type = Sampler3DArray;
        retval.byteSize = -1;
    }
    else if (string == "samplerCube")
    {
        retval.type = SamplerCube;
        retval.byteSize = -1;
    }
    else if (string == "samplerCubeArray")
    {
        retval.type = SamplerCubeArray;
        retval.byteSize = -1;
    }
    else if (string == "any")
    {
        retval.type = Any;
        retval.byteSize = -1;
    }
    else if (string == "anyvec")
    {
        retval.type = AnyVec;
        retval.byteSize = -1;
    }
	return retval;
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
VarType::ToString( const VarType& type )
{
	switch (type.type)
	{
	case Float:
		return "float";
	case Float2:
		return "vec2";
	case Float3:
		return "vec3";
	case Float4:
		return "vec4";
	case Int:
		return "int";
	case Int2:
		return "ivec2";
	case Int3:
		return "ivec3";
	case Int4:
		return "ivec4";
	case Bool:
		return "bool";
	case Bool2:
		return "bvec2";
	case Bool3:
		return "bvec3";
	case Bool4:
		return "bvec4";
	case Matrix2x2:
		return "mat2x2";
	case Matrix2x3:
		return "mat2x2";
	case Matrix2x4:
		return "mat2x4";
	case Matrix3x2:
		return "mat3x2";
	case Matrix3x3:
		return "mat3x3";
	case Matrix3x4:
		return "mat3x4";
	case Matrix4x2:
		return "mat4x2";
	case Matrix4x3:
		return "mat4x3";
	case Matrix4x4:
		return "mat4x4";
    case Sampler2D:
        return "sampler2D";
    case Sampler2DArray:
        return "sampler2DArray";
    case Sampler3D:
        return "sampler3D";
    case Sampler3DArray:
        return "sampler3DArray";
    case SamplerCube:
        return "samplerCube";
    case SamplerCubeArray:
        return "samplerCubeArray";
    case Any:
        return "any";
    case AnyVec:
        return "anyvec";
	}

	// this must never happen
	return "unknown";
}

//------------------------------------------------------------------------------
/**
*/
bool 
VarType::CanMap( const VarType& rhs ) const
{
    if (this->type == rhs.type) return true;
    else
    {
        if      (this->type == AnyVec && VarType::IsVector(rhs))    return true;
        else if (this->type == Any)                                 return true;
        else if (rhs.type == Any)                                   return true;
        else if (VarType::IsVector(*this) && rhs.type == AnyVec)    return true;
    }
    
	return false;
}

//------------------------------------------------------------------------------
/**
*/
bool 
VarType::IsVector( const VarType& type )
{
    switch (type.type)
    {
    case Float2:
    case Float3:
    case Float4:
    case Int2:
    case Int3:
    case Int4:
    case Bool2:
    case Bool3:
    case Bool4:
    case AnyVec:
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
uint 
VarType::VectorSize( const VarType& type )
{
    switch (type.type)
    {
    case Float2:
    case Int2:
    case Bool2:
        return 2;
    case Float3:
    case Int3:
    case Bool3:
        return 3;
    case Float4:
    case Int4:
    case Bool4:
        return 4;
    }
    return 1;
}

//------------------------------------------------------------------------------
/**
*/
VarType::ValueType 
VarType::ComponentType( const VarType& type )
{
    switch (type.type)
    {
    case Float:
    case Float2:
    case Float3:
    case Float4:
    case Matrix2x2:
    case Matrix2x3:
    case Matrix2x4:
    case Matrix3x2:
    case Matrix3x3:
    case Matrix3x4:
    case Matrix4x2:
    case Matrix4x3:
    case Matrix4x4:
        return Float;
    case Int:
    case Int2:
    case Int3:
    case Int4:
        return Int;
    case Bool:
    case Bool2:
    case Bool3:
    case Bool4:
        return Bool;
    }
    return Any;
}

//------------------------------------------------------------------------------
/**
*/
const bool 
VarType::IsA( const ValueType& type ) const
{
    return this->type == type;
}

} // namespace Nody