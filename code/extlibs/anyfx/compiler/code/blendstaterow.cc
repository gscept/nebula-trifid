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

}

//------------------------------------------------------------------------------
/**
*/
BlendStateRow::~BlendStateRow()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BlendStateRow::SetString( Expression* renderTarget, const std::string& flag, const std::string& value )
{
	this->flagType = StringFlagType;
	this->flag = flag;
	if (this->stringValue)
	{
		delete this->stringValue;
	}
	this->stringValue = new std::string(value);
	this->renderTarget = renderTarget;
}

//------------------------------------------------------------------------------
/**
*/
void 
BlendStateRow::SetExpression( Expression* renderTarget, const std::string& flag, Expression* expr )
{
	this->flagType = ExpressionFlagType;
	this->flag = flag;
	this->renderTarget = renderTarget;
	this->expr = expr;
}

} // namespace AnyFX