//------------------------------------------------------------------------------
//  generator.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "generator.h"

namespace AnyFX
{

Generator* Generator::instance = 0;
//------------------------------------------------------------------------------
/**
*/
Generator::Generator() :
	errorCount(0),
	warningCount(0)
{
	assert(0 == instance);
	instance = this;
}

//------------------------------------------------------------------------------
/**
*/
Generator::~Generator()
{
	assert(0 != instance);
	instance = 0;
}

//------------------------------------------------------------------------------
/**
*/
Generator* 
Generator::Instance()
{
	assert(0 != instance);
	return instance;
}

//------------------------------------------------------------------------------
/**
*/
void
Generator::Error(const std::string& error)
{
	if (this->errors.find(error) == this->errors.end())
	{
		this->errors.insert(error);
		this->errorBuffer += error + "\n";
		this->errorCount++;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Generator::Warning(const std::string& warning)
{
	if (this->warnings.find(warning) == this->warnings.end())
	{
		this->warnings.insert(warning);
		this->errorBuffer += warning + "\n";
		this->warningCount++;
	}	
}

//------------------------------------------------------------------------------
/**
*/
const Generator::Status 
Generator::GetStatus() const
{
	if (this->errorCount > 0)
	{
		return Failed;
	}

	return Success;
}

} // namespace AnyFX