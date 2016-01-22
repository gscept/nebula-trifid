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
	expr(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RenderStateRow::~RenderStateRow()
{
	// everything in here is deleted elsewhere
}

//------------------------------------------------------------------------------
/**
*/
void
RenderStateRow::SetString(const std::string& flag, const std::string& value)
{
	this->flagType = StringFlagType;
	this->flag = flag;
	this->stringValue = value;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderStateRow::SetExpression(const std::string& flag, Expression* expr)
{
	this->flagType = ExpressionFlagType;
	this->flag = flag;
	this->expr = expr;
}

} // namespace AnyFX