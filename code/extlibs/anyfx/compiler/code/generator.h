#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Generator
    
    The generator is responsible for generating target platform code.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <map>
#include <set>
#include "util.h"
#include "header.h"
namespace AnyFX
{
class Generator
{
public:
	enum Status
	{
		Success,
		Failed
	};

	/// constructor
	Generator();
	/// destructor
	virtual ~Generator();

	/// get singleton instance
	static Generator* Instance();

	/// posts generator error
	void Error(const std::string& error);
	/// posts generator warning
	void Warning(const std::string& warning);

	/// returns type checker error count
	const unsigned GetErrorCount() const;
	/// returns type checker warning count
	const unsigned GetWarningCount() const;
	/// increment error counter, use only for when output needs to be explicitly handled
	void BumpErrorCount();
	/// increment warning counter, use only for when output needs to be explicitly handled
	void BumpWarningCount();
	/// returns generator status
	const Status GetStatus() const;

	/// set header profile
	void SetHeader(const Header& header);
	/// get header profile
	const Header& GetHeader() const;

	/// get error buffer
	const std::string& GetErrorBuffer() const;

private:
	Header header;
	static Generator* instance;

	std::string errorBuffer;
	std::set<std::string> errors;
	std::set<std::string> warnings;

	unsigned errorCount;
	unsigned warningCount;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
Generator::SetHeader(const Header& header)
{
	this->header = header;
}

//------------------------------------------------------------------------------
/**
*/
inline const Header& 
Generator::GetHeader() const
{
	return this->header;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
Generator::GetErrorCount() const
{
	return this->errorCount;
}

//------------------------------------------------------------------------------
/**
*/
inline const unsigned 
Generator::GetWarningCount() const
{
	return this->warningCount;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Generator::BumpErrorCount()
{
	this->errorCount++;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
Generator::BumpWarningCount()
{
	this->warningCount++;
}

//------------------------------------------------------------------------------
/**
*/
inline const std::string& 
Generator::GetErrorBuffer() const
{
	return this->errorBuffer;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
