//------------------------------------------------------------------------------
//  programrow.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "programrow.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
ProgramRow::ProgramRow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ProgramRow::~ProgramRow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ProgramRow::SetString(const std::string& flag, const std::string& value)
{
	this->flag = flag;
	this->stringValue = value;
}

//------------------------------------------------------------------------------
/**
*/
void
ProgramRow::SetSubroutineMapping(const std::string& var, const std::string& implementation)
{
    this->subroutineMappings[var] = implementation;
}
} // namespace AnyFX