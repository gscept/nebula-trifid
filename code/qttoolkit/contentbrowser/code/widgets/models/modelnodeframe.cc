//------------------------------------------------------------------------------
//  modelnodeframe.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelnodeframe.h"

namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::ModelNodeFrame()
{
	/// create handlers
	this->itemHandler = ModelNodeHandler::Create();

	/// setup ui
	this->ui.setupUi(this);

	/// set item handler stuff
	this->itemHandler->SetUI(&this->ui);
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::~ModelNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeFrame::Discard()
{
	this->itemHandler->Discard();
	this->itemHandler = 0;
}
} // namespace Widgets