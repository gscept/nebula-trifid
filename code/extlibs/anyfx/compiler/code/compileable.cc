//------------------------------------------------------------------------------
//  compileable.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "compileable.h"
#include "util.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Compileable::Compileable() :
	hasErrors(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Compileable::~Compileable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Compileable::Destroy()
{
	// nothing to do here
}

//------------------------------------------------------------------------------
/**
*/
void 
Compileable::Setup()
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Compileable::TypeCheck( TypeChecker& typechecker )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Compileable::Generate( Generator& generator )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
void 
Compileable::Compile( BinWriter& writer )
{
	// override in subclass
}

//------------------------------------------------------------------------------
/**
*/
std::string 
Compileable::ErrorSuffix() const
{
	std::string result = AnyFX::Format("%d:%d in file %s", this->line, this->row, this->file.c_str());
	return result;
}
} // namespace AnyFX