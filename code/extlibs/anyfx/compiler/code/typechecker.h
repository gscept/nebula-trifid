#pragma once
//------------------------------------------------------------------------------
/**
    @class TypeChecker
    
    The type checker assures default values for variables match their types
	and also makes sure shader linkage is possible
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <map>
#include "util.h"
#include "header.h"

namespace AnyFX
{
class Symbol;
class TypeChecker
{
public:

	enum Status
	{
		Success,
        Warnings,
		Failed
	};

	/// constructor
	TypeChecker();
	/// destructor
	virtual ~TypeChecker();

	/// get singleton instance
	static TypeChecker* Instance();

    /// adds a rersevde symbol
    void AddReservedSymbol(Symbol* symbol);
	/// adds symbol to type checker
	bool AddSymbol(Symbol* symbol);
	/// returns true if symbol exists
	bool HasSymbol(const std::string& name);
	/// gets symbol from type checker
	Symbol* GetSymbol(const std::string& name);

	/// posts a type error
	void Error(const std::string& message);
	/// posts a type warning
	void Warning(const std::string& message);
	/// post a link error
	void LinkError(const std::string& message);
	/// post a link warning
	void LinkWarning(const std::string& message);

	/// returns type checker error count
	const unsigned GetErrorCount() const;
	/// returns type checker warning count
	const unsigned GetWarningCount() const;
	/// increment error counter, use only for when output needs to be explicitly handled
	void BumpErrorCount();
	/// increment warning counter, use only for when output needs to be explicitly handled
	void BumpWarningCount();
	/// returns type checker status
	const Status GetStatus() const;

	/// set header profile
	void SetHeader(const Header& header);
	/// get header profile
	const Header& GetHeader() const;

	/// get error buffer
	const std::string& GetErrorBuffer() const;
private:

	Header header;
	static TypeChecker* instance;

	std::string errorBuffer;
	unsigned errorCount;
	unsigned warningCount;
	std::map<std::string, Symbol*> symbols;
    std::map<std::string, Symbol*> reservedSymbols;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
TypeChecker::SetHeader( const Header& header )
{
	this->header = header;
}

//------------------------------------------------------------------------------
/**
*/
inline const Header& 
TypeChecker::GetHeader() const
{
	return this->header;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
TypeChecker::GetErrorCount() const
{
	return this->errorCount;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TypeChecker::BumpErrorCount()
{
	this->errorCount++;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
TypeChecker::GetWarningCount() const
{
	return this->warningCount;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
TypeChecker::BumpWarningCount()
{
	this->warningCount++;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
TypeChecker::GetErrorBuffer() const
{
	return this->errorBuffer;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
