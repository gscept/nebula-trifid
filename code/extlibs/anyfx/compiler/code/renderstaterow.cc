//------------------------------------------------------------------------------
//  drawstaterow.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "renderstaterow.h"
#include "types.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
RenderStateRow::RenderStateRow() :
	expr(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RenderStateRow::~RenderStateRow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderStateRow::SetString( const std::string& flag, const std::string& value )
{
	this->flagType = StringFlagType;
	this->flag = flag;
	if (this->stringValue)
	{
		delete this->stringValue;
	}
	this->stringValue = new std::string(value);
}

//------------------------------------------------------------------------------
/**
*/
void 
RenderStateRow::SetExpression( const std::string& flag, Expression* expr )
{
	this->flagType = ExpressionFlagType;
	this->flag = flag;
	this->expr = expr;
}

} // namespace AnyFX