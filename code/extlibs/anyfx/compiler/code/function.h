#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Function
    
    Describes a function header, which may or may not act as shader.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <vector>
#include "datatype.h"
#include "functionattribute.h"
#include "parameter.h"
#include "symbol.h"
#include "header.h"
namespace AnyFX
{
class Function : public Symbol
{
public:
	/// constructor
	Function();
	/// destructor
	virtual ~Function();
	
	/// set vector of function parameters
	void SetParameters(const std::vector<Parameter>& parameters);
	/// get number of parameters
	const unsigned GetNumParameters() const;
	/// get pointer to parameter at index
	Parameter* GetParameter(unsigned index);
    /// get pointer to parameter as constant
    const Parameter* GetParameter(unsigned index) const;

	/// set code
	void SetCode(const std::string& data);
	/// get code
	const std::string& GetCode() const;
	/// sets return type
	void SetReturnType(const DataType& type);
	/// get return type
	const DataType& GetReturnType() const;
	/// set on which line the code starts
	void SetCodeLine(unsigned line);
	/// get on which line the code starts
	const unsigned GetCodeLine() const;

	/// toggles that this function should be used as a shader
	void SetShader(bool b);
	/// gets if this function is a shader
	bool IsShader() const;

	/// returns true if function has the given integer flag
	bool HasIntFlag(const FunctionAttribute::IntFlag& flag);
	/// returns integer value
	int GetIntFlag(const FunctionAttribute::IntFlag& flag) const;
	/// returns true if function has float flag
	bool HasFloatFlag(const FunctionAttribute::FloatFlag& flag);
	/// returns float value
	float GetFloatFlag(const FunctionAttribute::FloatFlag& flag) const;
	/// returns true if function has bool flag
	bool HasBoolFlag(const FunctionAttribute::BoolFlag& flag);
	/// returns bool flag
	bool GetBoolFlag(const FunctionAttribute::BoolFlag& flag) const;
	/// returns true if function has string flag
	bool HasStringFlag(const FunctionAttribute::StringFlag& flag);
	/// returns string flag
	const std::string& GetStringFlag(const FunctionAttribute::StringFlag& flag) const;

	/// consumes attribute 
	void ConsumeAttribute(const FunctionAttribute& attr);

	/// typechecks function
	void TypeCheck(TypeChecker& typechecker);

    /// return the index of the file, basically, this is used with preprocessors to map a number to an actual physical file
    const int GetFileIndex() const;

	/// constructs a list of only inputs
	std::vector<Parameter*> GetInputParameters();
	/// constructs a list of only outputs
	std::vector<Parameter*> GetOutputParameters();
private:
    friend class Subroutine;
	friend class Effect;

	/// restores function (should only be applied if function is not bound as a shader)
	void Restore(const Header& header, int index);

	DataType returnType;
	unsigned codeLine;
	std::string code;
	std::vector<Parameter> parameters;
	bool shaderTarget;
    int fileIndex;

	Expression* intExpressions[FunctionAttribute::NumIntFlags];
	Expression* floatExpressions[FunctionAttribute::NumFloatFlags];

	bool intAttributeMask[FunctionAttribute::NumIntFlags];
	int intAttributes[FunctionAttribute::NumIntFlags];

	bool floatAttributeMask[FunctionAttribute::NumFloatFlags];
	float floatAttributes[FunctionAttribute::NumFloatFlags];

	bool boolAttributeMask[FunctionAttribute::NumBoolFlags];
	bool boolAttributes[FunctionAttribute::NumBoolFlags];

	bool stringAttributeMask[FunctionAttribute::NumStringFlags];
	std::string stringAttributes[FunctionAttribute::NumStringFlags];

	std::vector<std::string> invalidAttributes;
	std::vector<std::string> invalidAttributeValues;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
Function::GetNumParameters() const
{
	return this->parameters.size();
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Function::GetCode() const
{
	return this->code;
}

//------------------------------------------------------------------------------
/**
*/
inline const DataType& 
Function::GetReturnType() const
{
	return this->returnType;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Function::HasIntFlag( const FunctionAttribute::IntFlag& flag )
{
	return this->intAttributeMask[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline int 
Function::GetIntFlag( const FunctionAttribute::IntFlag& flag ) const
{
	return this->intAttributes[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Function::HasFloatFlag( const FunctionAttribute::FloatFlag& flag )
{
	return this->floatAttributeMask[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline float 
Function::GetFloatFlag( const FunctionAttribute::FloatFlag& flag ) const
{
	return this->floatAttributes[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Function::HasBoolFlag( const FunctionAttribute::BoolFlag& flag )
{
	return this->boolAttributeMask[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Function::GetBoolFlag( const FunctionAttribute::BoolFlag& flag ) const
{
	return this->boolAttributes[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Function::HasStringFlag( const FunctionAttribute::StringFlag& flag )
{
	return this->stringAttributeMask[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Function::GetStringFlag( const FunctionAttribute::StringFlag& flag ) const
{
	return this->stringAttributes[flag];
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Function::SetCodeLine( unsigned line )
{
	this->codeLine = line;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
Function::GetCodeLine() const
{
	return this->codeLine;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Function::SetShader( bool b )
{
	this->shaderTarget = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
Function::IsShader() const
{
	return this->shaderTarget;
}


//------------------------------------------------------------------------------
/**
*/
inline const int 
Function::GetFileIndex() const
{
    return this->fileIndex;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
