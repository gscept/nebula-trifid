//------------------------------------------------------------------------------
//  blendflag.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "blendstaterow.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
BlendStateRow::BlendStateRow() :
	expr(NULL),
	renderTarget(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BlendStateRow::~BlendStateRow()
{
	// everything in here is deleted elsewhere
}

//------------------------------------------------------------------------------
/**
*/
void
BlendStateRow::SetString(Expression* renderTarget, const std::string& flag, const std::string& value)
{
	this->flagType = StringFlagType;
	this->flag = flag;
	this->stringValue = value;
	this->renderTarget = renderTarget;
}

//------------------------------------------------------------------------------
/**
*/
void
BlendStateRow::SetExpression(Expression* renderTarget, const std::string& flag, Expression* expr)
{
	this->flagType = ExpressionFlagType;
	this->flag = flag;
	this->renderTarget = renderTarget;
	this->expr = expr;
}

} // namespace AnyFX