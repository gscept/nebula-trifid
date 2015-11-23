#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Compileable
    
    Base class for every type of data structure which can be compiled.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "binwriter.h"
#include <string>
namespace AnyFX
{
class TypeChecker;
class Generator;
class Compileable
{
public:
	/// constructor
	Compileable();
	/// destructor
	virtual ~Compileable();

	/// destroy symbol
	virtual void Destroy();

	/// set line
	void SetLine(unsigned line);
	/// get line
	unsigned GetLine() const;
	/// set character position
	void SetPosition(unsigned pos);
	/// get character position
	unsigned GetPosition() const;
	/// set source file
	void SetFile(const std::string& string);
	/// get source file
	const std::string& GetFile() const;
	/// get if the compileable object has an error
	const bool HasError() const;
	/// get error
	const std::string& GetError() const;

	/// formats an appropriate error suffix
	std::string ErrorSuffix();
	
	/// override to setup necessary information
	virtual void Setup();
	/// override to perform type checking
	virtual void TypeCheck(TypeChecker& typechecker);
	/// override to perform code generation
	virtual void Generate(Generator& generator);
	/// override to perform compilation
	virtual void Compile(BinWriter& writer);

protected:
	std::string error;
	std::string file;
	int line;
	int row;
	bool hasErrors;
	bool hasWarnings;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
Compileable::SetLine( unsigned line )
{
	this->line = line;
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Compileable::GetLine() const
{
	return this->line;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Compileable::SetPosition( unsigned pos )
{
	this->row = pos;
}

//------------------------------------------------------------------------------
/**
*/
inline unsigned 
Compileable::GetPosition() const
{
	return this->row;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Compileable::SetFile(const std::string& string)
{
	this->file = string;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Compileable::GetFile() const
{
	return this->file;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
Compileable::HasError() const
{
	return this->hasErrors;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Compileable::GetError() const
{
	return this->error;
}
} // namespace AnyFX
//------------------------------------------------------------------------------