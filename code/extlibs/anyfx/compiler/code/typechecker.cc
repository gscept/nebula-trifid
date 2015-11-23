//------------------------------------------------------------------------------
//  typechecker.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "typechecker.h"
#include "util.h"
#include "symbol.h"

namespace AnyFX
{

TypeChecker* TypeChecker::instance = 0;
//------------------------------------------------------------------------------
/**
*/
TypeChecker::TypeChecker() :
	errorCount(0),
	warningCount(0)
{
	assert(0 == instance);
	instance = this;
}

//------------------------------------------------------------------------------
/**
*/
TypeChecker::~TypeChecker()
{
	assert(0 != instance);
	instance = 0;
}

//------------------------------------------------------------------------------
/**
*/
TypeChecker* 
TypeChecker::Instance()
{
	assert(0 != instance);
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
bool
TypeChecker::AddSymbol(Symbol* symbol)
{
	if (this->symbols.find(symbol->GetName()) != this->symbols.end())
	{
		Symbol* origSymbol = this->symbols[symbol->GetName()];
        std::string err;
        if (origSymbol->IsReserved())   err = Format("Symbol '%s' is a reserved name and can not be used.", symbol->GetName().c_str());
        else                            err = Format("Symbol '%s' redefinition at %d in %s. Previously defined near row %d in %s\n", symbol->GetName().c_str(), symbol->GetLine(), symbol->GetFile().c_str(), origSymbol->GetLine(), origSymbol->GetFile().c_str());
		this->Error(err);
		return false;	
	}
	else
	{
		this->symbols[symbol->GetName()] = symbol;
		return true;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
TypeChecker::HasSymbol(const std::string& name)
{
	return this->symbols.find(name) != this->symbols.end();
}

//------------------------------------------------------------------------------
/**
*/
Symbol*
TypeChecker::GetSymbol(const std::string& name)
{
	if (this->symbols.find(name) == this->symbols.end())
	{
		std::string err = Format("Symbol '%s' is not defined\n", name.c_str());
		this->Error(err);
		return NULL;
	}
	else
	{
		return this->symbols[name];
	}
}

//------------------------------------------------------------------------------
/**
*/
void
TypeChecker::Error(const std::string& message)
{
	this->errorBuffer += message + "\n";
	this->errorCount++;
}

//------------------------------------------------------------------------------
/**
*/
void
TypeChecker::Warning(const std::string& message)
{
	this->errorBuffer += message + "\n";
	this->warningCount++;
}

//------------------------------------------------------------------------------
/**
*/
void
TypeChecker::LinkError(const std::string& message)
{
	this->errorBuffer += message + "\n";
	this->errorCount++;
}

//------------------------------------------------------------------------------
/**
*/
void
TypeChecker::LinkWarning(const std::string& message)
{
	this->errorBuffer += message + "\n";
	this->warningCount++;
}

//------------------------------------------------------------------------------
/**
*/
const TypeChecker::Status 
TypeChecker::GetStatus() const
{
	if (this->errorCount > 0)
	{
		return Failed;
	}
    else if (this->warningCount > 0)
    {
        return Warnings;
    }
    else
    {
        return Success;
    }
}

} // namespace AnyFX