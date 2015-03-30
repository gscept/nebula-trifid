//------------------------------------------------------------------------------
//  datatype.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "datatype.h"
#include "util.h"
#include "typechecker.h"
#include "structure.h"


namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
DataType::DataType() :
	style(Generic),
	type(Undefined)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
DataType::~DataType()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
std::string 
DataType::ToGLSLType( const DataType& type )
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
	case Double:
		return "double";
	case Double2:
		return "dvec2";
	case Double3:
		return "dvec3";
	case Double4:
		return "dvec4";
	case Integer:
		return "int";
	case Integer2:
		return "ivec2";
	case Integer3:
		return "ivec3";
	case Integer4:
		return "ivec4";
	case UInteger:
		return "uint";
	case UInteger2:
		return "uvec2";
	case UInteger3:
		return "uvec3";
	case UInteger4:
		return "uvec4";
	case Short:
		return "short";
	case Short2:
		return "svec2";
	case Short3:
		return "svec3";
	case Short4:
		return "svec4";
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
		return "mat2x3";
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
	case Sampler1D:
		return "sampler1D";
	case Sampler1DArray:
		return "sampler1DArray";
	case Sampler2D:
		return "sampler2D";
	case Sampler2DArray:
		return "sampler2DArray";
	case Sampler2DMS:
		return "sampler2DMS";
	case Sampler2DMSArray:
		return "sampler2DMSArray";
	case Sampler3D:
		return "sampler3D";
	case SamplerCube:
		return "samplerCube";
	case SamplerCubeArray:
		return "samplerCubeArray";
	case Image1D:
		return "image1D";
	case Image1DArray:
		return "image1DArray";
	case Image2D:
		return "image2D";
	case Image2DArray:
		return "image2DArray";
	case Image3D:
		return "image3D";
	case ImageCube:
		return "imageCube";
	case ImageCubeArray:
		return "imageCubeArray";
	case Image2DMS:
		return "image2DMS";
	case Image2DMSArray:
		return "image2DMSArray";
	case Void:
		return "void";
	case UserType:
		return type.name;
	case Undefined:
	default:
		return "undefined";
	}
}

//------------------------------------------------------------------------------
/**
*/
std::string 
DataType::ToHLSLType( const DataType& type )
{
	switch (type.type)
	{
	case Float:
		return "float";
	case Float2:
		return "float2";
	case Float3:
		return "float3";
	case Float4:
		return "float4";
	case Double:
		return "double";
	case Double2:
		return "double2";
	case Double3:
		return "double3";
	case Double4:
		return "double4";
	case Integer:
		return "int";
	case Integer2:
		return "int2";
	case Integer3:
		return "int3";
	case Integer4:
		return "int4";
	case UInteger:
		return "uint";
	case UInteger2:
		return "uint2";
	case UInteger3:
		return "uint3";
	case UInteger4:
		return "uint4";
	case Short:
		return "short";
	case Short2:
		return "short2";
	case Short3:
		return "short3";
	case Short4:
		return "short4";
	case Bool:
		return "bool";
	case Bool2:
		return "bool2";
	case Bool3:
		return "bool3";
	case Bool4:
		return "bool4";
	case Matrix2x2:
		return "float2x2";
	case Matrix2x3:
		return "float2x3";
	case Matrix2x4:
		return "float2x4";
	case Matrix3x2:
		return "float3x2";
	case Matrix3x3:
		return "float3x3";
	case Matrix3x4:
		return "float3x4";
	case Matrix4x2:
		return "float4x2";
	case Matrix4x3:
		return "float4x3";
	case Matrix4x4:
		return "float4x4";
	case Sampler1D:
		return "Texture1D";
	case Sampler1DArray:
		return "Texture1DArray";
	case Sampler2D:
		return "Texture2D";
	case Sampler2DArray:
		return "Texture2DArray";
	case Sampler2DMS:
		return "Texture2DMS";
	case Sampler2DMSArray:
		return "Texture2DMSArray";
	case Sampler3D:
		return "Texture3D";
	case SamplerCube:
		return "TextureCube";
	case SamplerCubeArray:
		return "TextureCubeArray";
	case Image1D:
		return "RWTexture1D";
	case Image1DArray:
		return "RWTexture1DArray";
	case Image2D:
		return "RWTexture2D";
	case Image2DArray:
		return "RWTexture2DArray";
	case Image3D:
		return "RWTexture3D";
	case Void:
		return "void";
	case UserType:
		return type.name;

		// these types are undefined for hlsl
	case ImageCube:
		return "undefined";
	case ImageCubeArray:
		return "undefined";
	case Image2DMS:
		return "undefined";
	case Image2DMSArray:
		return "undefined";
	case Undefined:
	default:
		return "undefined";
	}
}

//------------------------------------------------------------------------------
/**
*/
std::string 
DataType::ToProfileType( const DataType& dtype, const Header::Type& ttype )
{
	switch (ttype)
	{
	case Header::HLSL:
		return ToHLSLType(dtype);
	case Header::GLSL:
		return ToGLSLType(dtype);
	default:
		Error("Type cannot be converted");
		return "";
	}
}

//------------------------------------------------------------------------------
/**
*/
std::string 
DataType::ToSignature( const DataType& type )
{
	switch (type.type)
	{
	case Float:
		return "f";
	case Float2:
		return "f2";
	case Float3:
		return "f3";
	case Float4:
		return "f4";
	case Double:
		return "d";
	case Double2:
		return "d2";
	case Double3:
		return "d3";
	case Double4:
		return "d4";
	case Integer:
		return "i";
	case Integer2:
		return "i2";
	case Integer3:
		return "i3";
	case Integer4:
		return "i4";
	case UInteger:
		return "ui";
	case UInteger2:
		return "ui2";
	case UInteger3:
		return "ui3";
	case UInteger4:
		return "ui4";
	case Short:
		return "s";
	case Short2:
		return "s2";
	case Short3:
		return "s3";
	case Short4:
		return "s4";
	case Bool:
		return "b";
	case Bool2:
		return "b2";
	case Bool3:
		return "b3";
	case Bool4:
		return "b4";
	case Matrix2x2:
		return "m22";
	case Matrix2x3:
		return "m23";
	case Matrix2x4:
		return "m24";
	case Matrix3x2:
		return "m32";
	case Matrix3x3:
		return "m33";
	case Matrix3x4:
		return "m34";
	case Matrix4x2:
		return "m42";
	case Matrix4x3:
		return "m43";
	case Matrix4x4:
		return "m44";
	case Sampler1D:
		return "sam1d";
	case Sampler1DArray:
		return "sam1da";
	case Sampler2D:
		return "sam2d";
	case Sampler2DArray:
		return "sam2da";
	case Sampler2DMS:
		return "sam2dms";
	case Sampler2DMSArray:
		return "sam2dmsa";
	case Sampler3D:
		return "sam3d";
	case SamplerCube:
		return "samc";
	case SamplerCubeArray:
		return "samca";
	case Image1D:
		return "ima1d";
	case Image1DArray:
		return "ima1da";
	case Image2D:
		return "ima2d";
	case Image2DArray:
		return "ima2da";
	case Image3D:
		return "ima3d";
	case ImageCube:
		return "imac";
	case ImageCubeArray:
		return "imaca";
	case Image2DMS:
		return "ima2dms";
	case Image2DMSArray:
		return "ima2dmsa";
	case Void:
		return "v";
	case UserType:
		return type.name;
	case Undefined:
	default:
		return "undefined";
	}
}

//------------------------------------------------------------------------------
/**
*/
std::string 
DataType::ToString( const DataType& type )
{
	switch (type.type)
	{
	case Float:
		return "float";
	case Float2:
		return "float vector [2]";
	case Float3:
		return "float vector [3]";
	case Float4:
		return "float vector [4]";
	case Double:
		return "double";
	case Double2:
		return "double vector [2]";
	case Double3:
		return "double vector [3]";
	case Double4:
		return "double vector [4]";
	case Integer:
		return "int";
	case Integer2:
		return "int vector [2]";
	case Integer3:
		return "int vector [3]";
	case Integer4:
		return "int vector [4]";
	case UInteger:
		return "unsigned int";
	case UInteger2:
		return "unsigned int vector [2]";
	case UInteger3:
		return "unsigned int vector [3]";
	case UInteger4:
		return "unsigned int vector [4]";
	case Short:
		return "short";
	case Short2:
		return "short vector [2]";
	case Short3:
		return "short vector [3]";
	case Short4:
		return "short vector [4]";
	case Bool:
		return "bool";
	case Bool2:
		return "bool vector [2]";
	case Bool3:
		return "bool vector [3]";
	case Bool4:
		return "bool vector [4]";
	case Matrix2x2:
		return "matrix [2x2]";
	case Matrix2x3:
		return "matrix [2x3]";
	case Matrix2x4:
		return "matrix [2x4]";
	case Matrix3x2:
		return "matrix [3x2]";
	case Matrix3x3:
		return "matrix [3x3]";
	case Matrix3x4:
		return "matrix [3x4]";
	case Matrix4x2:
		return "matrix [4x2]";
	case Matrix4x3:
		return "matrix [4x3]";
	case Matrix4x4:
		return "matrix [4x4]";
	case Sampler1D:
		return "texture sampler [1D]";
	case Sampler1DArray:
		return "texture sampler [1D-Array]";
	case Sampler2D:
		return "texture sampler [2D]";
	case Sampler2DArray:
		return "texture sampler [2D-Array]";
	case Sampler2DMS:
		return "texture sampler [2D-Multisample]";
	case Sampler2DMSArray:
		return "texture sampler [2D-Multisample-Array]";
	case Sampler3D:
		return "texture sampler [3D]";
	case SamplerCube:
		return "texture sampler [Cube]";
	case SamplerCubeArray:
		return "texture sampler [Cube-Array]";
	case Image1D:
		return "image [1D]";
	case Image1DArray:
		return "image [1D-Array]";
	case Image2D:
		return "image [2D]";
	case Image2DArray:
		return "image [2D-Array]";
	case Image3D:
		return "image [3D]";
	case ImageCube:
		return "image [Cube]";
	case ImageCubeArray:
		return "image [Cube-Array]";
	case Image2DMS:
		return "image [2D-Multisample]";
	case Image2DMSArray:
		return "image [2D-Multisample-Array]";
	case Void:
		return "void";
	case String:
		return "string";
	case UserType:
		return type.name;
	case Undefined:
	default:
		return "undefined";
	}
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
DataType::ToVectorSize( const DataType& type )
{
	switch (type.type)
	{
	case Double:
	case Float:
	case Integer:
	case UInteger:
	case Short:
	case Bool:
		return 1;
	case Float2:
	case Double2:
	case Integer2:
	case UInteger2:
	case Short2:
	case Bool2:
		return 2;
	case Double3:
	case Float3:
	case Integer3:
	case UInteger3:
	case Short3:
	case Bool3:
		return 3;
	case Double4:
	case Float4:
	case Integer4:
	case UInteger4:
	case Short4:
	case Bool4:
		return 4;
	case Matrix2x2:
		return 4;
	case Matrix2x3:
		return 6;
	case Matrix2x4:
		return 8;
	case Matrix3x2:
		return 6;
	case Matrix3x3:
		return 9;
	case Matrix3x4:
		return 12;
	case Matrix4x2:
		return 8;
	case Matrix4x3:
		return 12;
	case Matrix4x4:
		return 16;
	case Sampler1D:
	case Sampler1DArray:
	case Sampler2D:
	case Sampler2DArray:
	case Sampler2DMS:
	case Sampler2DMSArray:
	case Sampler3D:
	case SamplerCube:
	case SamplerCubeArray:
	case Image1D:
	case Image1DArray:
	case Image2D:
	case Image2DArray:
	case Image3D:
	case ImageCube:
	case ImageCubeArray:
	case Image2DMS:
	case Image2DMSArray:
	case Void:
	case UserType:
	case Undefined:
	default:
		return 0;
	}
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
DataType::ToByteSize( const DataType& type )
{
    switch (type.GetType())
    {
    case DataType::Float:
        return sizeof(float);
    case DataType::Float2:
        return 2 * sizeof(float);
    case DataType::Float3:
        return 3 * sizeof(float);
    case DataType::Float4:
        return 4 * sizeof(float);
    case DataType::Double:
        return sizeof(double);
    case DataType::Double2:
        return 2 * sizeof(double);
    case DataType::Double3:
        return 3 * sizeof(double);
    case DataType::Double4:
        return 4 * sizeof(double);
    case DataType::UInteger:
    case DataType::Integer:
        return sizeof(int);
    case DataType::UInteger2:
    case DataType::Integer2:
        return 2 * sizeof(int);
    case DataType::UInteger3:
    case DataType::Integer3:
        return 3 * sizeof(int);
    case DataType::UInteger4:
    case DataType::Integer4:
        return 4 * sizeof(int);
    case DataType::Short:
        return sizeof(short);
    case DataType::Short2:
        return 2 * sizeof(short);
    case DataType::Short3:
        return 3 * sizeof(short);
    case DataType::Short4:
        return 4 * sizeof(short);
    case DataType::Bool:
        return sizeof(bool);
    case DataType::Bool2:
        return 2 * sizeof(bool);
    case DataType::Bool3:
        return 3 * sizeof(bool);
    case DataType::Bool4:
        return 4 * sizeof(bool);
    case DataType::Matrix2x2:
        return 4 * sizeof(float);
    case DataType::Matrix2x3:
        return 6 * sizeof(float);
    case DataType::Matrix2x4:
        return 8 * sizeof(float);
    case DataType::Matrix3x2:
        return 6 * sizeof(float);
    case DataType::Matrix3x3:
        return 9 * sizeof(float);
    case DataType::Matrix3x4:
        return 12 * sizeof(float);
    case DataType::Matrix4x2:
        return 8 * sizeof(float);
    case DataType::Matrix4x3:
        return 12 * sizeof(float);
    case DataType::Matrix4x4:
        return 16 * sizeof(float);
    case DataType::UserType:
        {
            TypeChecker* typeChecker = TypeChecker::Instance();
            if (typeChecker->HasSymbol(type.GetName()))
            {
                Symbol* sym = typeChecker->GetSymbol(type.GetName());
                if (sym->GetType() == Symbol::StructureType)
                {
                    Structure* struc = (Structure*)sym;
                    return struc->CalculateSize();
                }
                
            }    
        }
    default:		// this is for all special types, such as texture handles etc.
        return sizeof(int);
    }
}

//------------------------------------------------------------------------------
/**
*/
DataType
DataType::ToPrimitiveType(const DataType& type)
{
	DataType ret;
	switch (type.GetType())
	{
	case DataType::Float:
	case DataType::Float2:
	case DataType::Float3:
	case DataType::Float4:
		ret.type = DataType::Float;

	case DataType::Double:
	case DataType::Double2:
	case DataType::Double3:
	case DataType::Double4:
		ret.type = DataType::Double;

	case DataType::UInteger:
	case DataType::UInteger2:
	case DataType::UInteger3:
	case DataType::UInteger4:
		ret.type = DataType::UInteger;

	case DataType::Integer:
	case DataType::Integer2:
	case DataType::Integer3:
	case DataType::Integer4:
		ret.type = DataType::Integer;

	case DataType::Short:
	case DataType::Short2:
	case DataType::Short3:
	case DataType::Short4:
		ret.type = DataType::Short;

	case DataType::Bool:
	case DataType::Bool2:
	case DataType::Bool3:
	case DataType::Bool4:
		ret.type = DataType::Bool;

	case DataType::Matrix2x2:
	case DataType::Matrix2x3:
	case DataType::Matrix2x4:
	case DataType::Matrix3x2:
	case DataType::Matrix3x3:
	case DataType::Matrix3x4:
	case DataType::Matrix4x2:
	case DataType::Matrix4x3:
	case DataType::Matrix4x4:
		ret.type = DataType::Float;

	case DataType::UserType:
		ret.type = DataType::UserType;

	default:		// this is for all special types, such as texture handles etc.
		ret.type = DataType::Integer;
	}
	return ret;
}

} // namespace AnyFX
