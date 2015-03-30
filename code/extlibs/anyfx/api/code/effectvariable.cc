//------------------------------------------------------------------------------
//  effectvariable.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "./effectvariable.h"
#include "internal/internaleffectvariable.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
EffectVariable::EffectVariable() :
	internalVariable(0),
	currentValue(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
EffectVariable::~EffectVariable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::Discard()
{
    this->internalVariable->Release();
	this->internalVariable = 0;
}

//------------------------------------------------------------------------------
/**
*/
const std::string& 
EffectVariable::GetName() const
{
	return this->internalVariable->GetName();
}

//------------------------------------------------------------------------------
/**
*/
const VariableType& 
EffectVariable::GetType() const
{
	return this->internalVariable->GetType();
}

//------------------------------------------------------------------------------
/**
*/
const bool 
EffectVariable::IsInVarblock() const
{
	return this->internalVariable->IsInVarblock();
}

//------------------------------------------------------------------------------
/**
*/
const bool 
EffectVariable::IsSubroutine() const
{
    return this->internalVariable->IsSubroutine();
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::Commit()
{
	this->internalVariable->Commit();
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetFloat( float f )
{
	this->internalVariable->SetFloat(f);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetFloat2( const float* vec )
{
	this->internalVariable->SetFloat2(vec);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetFloat4( const float* vec )
{
	this->internalVariable->SetFloat4(vec);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetFloatArray( const float* f, size_t count )
{
	this->internalVariable->SetFloatArray(f, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetFloat2Array( const float* f, size_t count )
{
	this->internalVariable->SetFloat2Array(f, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetFloat4Array( const float* f, size_t count )
{
	this->internalVariable->SetFloat4Array(f, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetInt( int i )
{
	this->internalVariable->SetInt(i);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetInt2( const int* vec )
{
	this->internalVariable->SetInt2(vec);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetInt4( const int* vec )
{
	this->internalVariable->SetInt4(vec);
}
//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetIntArray( const int* i, size_t count )
{
	this->internalVariable->SetIntArray(i, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetInt2Array( const int* i, size_t count )
{
	this->internalVariable->SetInt2Array(i, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetInt4Array( const int* i, size_t count )
{
	this->internalVariable->SetInt4Array(i, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetBool( bool b )
{
	this->internalVariable->SetBool(b);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetBool2( const bool* vec )
{
	this->internalVariable->SetBool2(vec);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetBool4( const bool* vec )
{
	this->internalVariable->SetBool4(vec);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetBoolArray( const bool* b, size_t count )
{
	this->internalVariable->SetBoolArray(b, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetBool2Array( const bool* b, size_t count )
{
	this->internalVariable->SetBool2Array(b, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetBool4Array( const bool* b, size_t count )
{
	this->internalVariable->SetBool4Array(b, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetMatrix( const float* mat )
{
	this->internalVariable->SetMatrix(mat);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetMatrixArray( const float* mat, size_t count )
{
	this->internalVariable->SetMatrixArray(mat, count);
}

//------------------------------------------------------------------------------
/**
*/
void 
EffectVariable::SetTexture( void* handle )
{
	this->internalVariable->SetTexture(handle);
}

//------------------------------------------------------------------------------
/**
*/
void
EffectVariable::SetTextureHandle(void* handle)
{
	// hmm, code should go here, but it hasn't
}

//------------------------------------------------------------------------------
/**
*/
std::string 
EffectVariable::TypeToString( const VariableType& type )
{
	switch (type)
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
		return "s1d";
	case Sampler1DArray:
		return "s1da";
	case Sampler2D:
		return "s2d";
	case Sampler2DArray:
		return "s2da";
	case Sampler2DMS:
		return "s2dm";
	case Sampler2DMSArray:
		return "s2dma";
	case Sampler3D:
		return "s3d";
	case SamplerCube:
		return "sc";
	case SamplerCubeArray:
		return "sca";
	case Image1D:
		return "i1d";
	case Image1DArray:
		return "i1da";
	case Image2D:
		return "i2d";
	case Image2DArray:
		return "i2da";
	case Image2DMS:
		return "i2dm";
	case Image2DMSArray:
		return "i2dma";
	case Image3D:
		return "i3d";
	case ImageCube:
		return "ic";
	case ImageCubeArray:
		return "ica";
	case Void:
		return "v";
	case String:
		return "str";
	case UserType:
		return "user";
	default:
		return "undefined";
	}
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
EffectVariable::TypeToByteSize( const VariableType& type )
{
	switch (type)
	{
	case Float:
		return sizeof(float);
	case Float2:
		return 2 * sizeof(float);
	case Float3:
		return 3 * sizeof(float);
	case Float4:
		return 4 * sizeof(float);
	case Double:
		return sizeof(double);
	case Double2:
		return 2 * sizeof(double);
	case Double3:
		return 3 * sizeof(double);
	case Double4:
		return 4 * sizeof(double);
	case UInteger:
	case Integer:
		return sizeof(int);
	case UInteger2:
	case Integer2:
		return 2 * sizeof(int);
	case UInteger3:
	case Integer3:
		return 3 * sizeof(int);
	case UInteger4:
	case Integer4:
		return 4 * sizeof(int);
	case Short:
		return sizeof(short);
	case Short2:
		return 2 * sizeof(short);
	case Short3:
		return 3 * sizeof(short);
	case Short4:
		return 4 * sizeof(short);

		// GPUs treat bools as integers, so just assume the size is 1 byte
	case Bool:
		return sizeof(bool);			
	case Bool2:
		return 2 * sizeof(bool);
	case Bool3:
		return 3 * sizeof(bool);
	case Bool4:
		return 4 * sizeof(bool);
	case Matrix2x2:
		return 4 * sizeof(float);
	case Matrix2x3:
		return 6 * sizeof(float);
	case Matrix2x4:
		return 8 * sizeof(float);
	case Matrix3x2:
		return 6 * sizeof(float);
	case Matrix3x3:
		return 9 * sizeof(float);
	case Matrix3x4:
		return 12 * sizeof(float);
	case Matrix4x2:
		return 8 * sizeof(float);
	case Matrix4x3:
		return 12 * sizeof(float);
	case Matrix4x4:
		return 16 * sizeof(float);
	default:		// this is for all special types, such as texture handles etc.
		return sizeof(int);
	}
}

//------------------------------------------------------------------------------
/**
*/
const bool 
EffectVariable::IsActive() const
{
	return this->internalVariable->active;
}

} // namespace AnyFX