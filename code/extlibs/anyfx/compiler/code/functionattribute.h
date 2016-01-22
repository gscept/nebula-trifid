#pragma once
//------------------------------------------------------------------------------
/**
    @class FunctionAttribute
    
    The function attribute type handles storing function attributes, but only one at a time.
    
    (C) 2013 gscept
*/
//------------------------------------------------------------------------------
#include <string>
#include "compileable.h"
#include <assert.h>
#include "expressions/expression.h"
namespace AnyFX
{

class FunctionAttribute : public Compileable
{
public:

	enum FlagType
	{
		ExpressionFlagType,
		StringFlagType,
		BoolFlagType,

		InvalidFlagType,

		NumFlagTypes
	};

	enum IntFlag
	{
		Topology,
		WindingOrder,
		PartitionMethod,
		InputPrimitive,
		OutputPrimitive,
		InputVertices,
		OutputVertices,
		MaxVertexCount,
		Instances,
		LocalSizeX,
		LocalSizeY,
		LocalSizeZ,

		NumIntFlags,

		InvalidIntFlag
	};

	enum TopologyMode
	{
		Triangle,
		Quad,
		Line,
		Point
	};

	enum WindingMode
	{
		CW,
		CCW
	};

	enum PartitionMode
	{
		Integer,
		Even,
		Odd,
		Pow
	};


	enum InputPrimitiveMode
	{
		IPoints,
		ILines,
		ILinesAdjacent,
		ITriangles,
		ITrianglesAdjacent
	};

	enum OutputPrimitiveMode
	{
		OPoints,
		OLineStrip,
		OTriangleStrip
	};

	enum FloatFlag
	{
		MaxTessellation,

		NumFloatFlags,

		InvalidFloatFlag
	};

	enum StringFlag
	{
		PatchFunction,

		NumStringFlags,

		InvalidStringFlag
	};

	enum BoolFlag
	{
		EarlyDepth,

		NumBoolFlags,

		InvalidBoolFlag
	};

	/// constructor
	FunctionAttribute();
	/// destructor
	virtual ~FunctionAttribute();

	/// sets bool flag
	void SetBool(const std::string& flag, bool value);
	/// gets bool flag
	bool GetBool() const;
	/// sets string flag
	void SetString(const std::string& flag, const std::string& value);
	/// gets string
	const std::string& GetString() const;

	/// sets value expression
	void SetExpression(const std::string& flag, Expression* expr);
	/// gets value expression
	Expression* GetExpression() const;

	/// get flag type
	const FlagType& GetFlagType() const;
	/// get flag
	const std::string& GetFlag() const;

private:
	FlagType flagType;
	std::string flag;

	Expression* expr;
	bool boolValue;
	std::string stringValue;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const FunctionAttribute::FlagType& 
FunctionAttribute::GetFlagType() const
{
	return this->flagType;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
FunctionAttribute::GetFlag() const
{
	return this->flag;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
FunctionAttribute::GetBool() const
{
	assert(this->flagType == BoolFlagType);
	return this->boolValue;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
FunctionAttribute::GetString() const
{
	assert(this->flagType == StringFlagType);
	return this->stringValue;
}

//------------------------------------------------------------------------------
/**
*/
inline Expression* 
FunctionAttribute::GetExpression() const
{
	return this->expr;
}

} // namespace AnyFX
//------------------------------------------------------------------------------