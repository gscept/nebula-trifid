#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Symbol
    
    Basic object representation, contains basic information for an AnyFX symbol.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "compileable.h"
#include <string>
namespace AnyFX
{
class Symbol : public Compileable
{
public:

	enum Type
	{
		ProgramType,
		RenderStateType,
		SamplerType,
		VariableType,
		VarblockType,
        VarbufferType,
        SubroutineType,
		FunctionType,
		ConstantType,
		StructureType,

		NumSymbolTypes
	};

	/// constructor
	Symbol();
	/// destructor
	virtual ~Symbol();

	/// set name of program
	void SetName(const std::string& name);
	/// gets name of program
	const std::string& GetName() const;
    /// set if symbol is a reserved (by the compiler)
    void SetReserved(bool b);
    /// get if symbol is reserved
    const bool IsReserved() const;

	/// returns type of symbol
	const Type& GetType() const;

protected:
    bool reserved;
	std::string name;
	Type symbolType;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Symbol::SetName( const std::string& name )
{
	this->name = name;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Symbol::GetName() const
{
	return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Symbol::SetReserved(bool b)
{
    this->reserved = b;
    this->file = "<intrinsic>";
    this->line = 0;    
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
Symbol::IsReserved() const
{
    return this->reserved;
}

//------------------------------------------------------------------------------
/**
*/
inline const Symbol::Type& 
Symbol::GetType() const
{
	return this->symbolType;
}

} // namespace AnyFX
//------------------------------------------------------------------------------