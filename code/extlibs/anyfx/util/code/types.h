#pragma once
//------------------------------------------------------------------------------
/**
    types.h
    
    Contains data types used to maintain initializers and data types
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------

namespace AnyFX
{

template<typename T, int S>
struct vector
{
	T v[S];
};

template<typename T, int R, int C>
struct matrix
{
	T m[R][C];
};

// this enumeration is globally available seeing as it is used by many different parser types
enum ComparisonFunction
{
	Never,
	Less,
	LEqual,
	Greater,
	GEqual,
	Equal,
	NEqual,
	Always,

	NumComparisonFunctions,

	InvalidComparisonFunction
};

}
