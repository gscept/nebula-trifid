//------------------------------------------------------------------------------
//  physicsnodeframe.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physicsnodeframe.h"

namespace Widgets
{

//------------------------------------------------------------------------------
/**
*/
PhysicsNodeFrame::PhysicsNodeFrame()
{
	/// create handlers
	this->itemHandler = PhysicsNodeHandler::Create();

	/// setup ui
	this->ui.setupUi(this);

	/// set item handler stuff
	this->itemHandler->SetUI(&this->ui);
}

//------------------------------------------------------------------------------
/**
*/
PhysicsNodeFrame::~PhysicsNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PhysicsNodeFrame::Discard()
{
	this->itemHandler->Discard();
	this->itemHandler = 0;
}
} // namespace Widgets