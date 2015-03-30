#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::VariableType
    
    Holds shared enum for variable types
    
    (C) 2013 Gustav Sterbrant
*/

//------------------------------------------------------------------------------

// some thought it would be a good idea to define bool somewhere in some lib, so we undefine it here...
#undef Bool

namespace AnyFX
{

// make sure to keep this enum matched with the one in the compiler
enum VariableType
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

} // namespace AnyFX
//------------------------------------------------------------------------------
